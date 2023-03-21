/**
* Method for running and evaluating experiments on the SBM graphs.
*/
#include <chrono>
#include <iostream>
#include <stag/graph.h>
#include <stag/cluster.h>
#include <stag/graphio.h>

#include "generate.h"
#include "experiments.h"

/**
 * Get the symmetric difference of the given cluster with the ground truth
 * cluster whose first node id is given by the starting_vertex parameter.
 *
 * This method assumes that the cluster is of size 1000.
 */
stag_int get_sym_diff(stag_int starting_vertex, std::vector<stag_int>& cluster) {
  // We expect 1000 nodes in the cluster. For every correct node, we will decrease
  // the symmetric difference by 1 and for every incorrect node we will increase
  // by 1.
  stag_int sym_diff = 1000;

  for (stag_int v : cluster) {
    // There are 1000 nodes in the correct cluster, check that this node is in
    // the correct range.
    if (v >= starting_vertex && v < starting_vertex + 1000) {
      // This is a correct node
      sym_diff--;
    } else {
      // This is an incorrect node
      sym_diff++;
    }
  }

  return sym_diff;
}

/**
 * A structure representing the information returned by a single run of an
 * algorithm.
 */
struct ExperimentalResult {
  // The running time of the algorithm
  stag_int time;

  // The symmetric difference with the true cluster
  stag_int symmetric_distance;

  // The size of the returned cluster
  stag_int cluster_size;
};

/**
 * Given an adjacencylist file and a starting vertex (which should be the first
 * index of the cluster in the SBM), evaluate the performance of on-disk
 * local clustering.
 */
ExperimentalResult evaluate_ondisk_clustering(std::string& filename,
                                              stag_int start_vertex) {
  auto start = std::chrono::high_resolution_clock::now();
  stag::AdjacencyListLocalGraph g = stag::AdjacencyListLocalGraph(filename);
  std::vector<stag_int> cluster = stag::local_cluster(&g,
                                                      start_vertex,
                                                      20000);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop-start);
  stag_int sym_diff = get_sym_diff(start_vertex, cluster);
  return {duration.count(), sym_diff, (stag_int) cluster.size()};
}

/**
 * Given an adjacencylist file and a starting vertex (which should be the first
 * index of the cluster in the SBM), evaluate the performance of in-memory
 * local clustering.
 */
ExperimentalResult evaluate_inmemory_clustering(std::string& filename,
                                                stag_int start_vertex) {
  auto start = std::chrono::high_resolution_clock::now();
  stag::Graph g = stag::load_adjacencylist(filename);
  std::vector<stag_int> cluster = stag::local_cluster(&g,
                                                      start_vertex,
                                                      20000);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop-start);
  stag_int sym_diff = get_sym_diff(start_vertex, cluster);
  return {duration.count(), sym_diff, (stag_int) cluster.size()};
}

/**
 * Given a vector of results, compute the average result.
 */
ExperimentalResult compute_aggregate_result(std::vector<ExperimentalResult>& results) {
  auto num_trials = (stag_int) results.size();
  ExperimentalResult agg{0, 0, 0};

  for (ExperimentalResult res : results) {
    agg.time += res.time;
    agg.symmetric_distance += res.symmetric_distance;
    agg.cluster_size += res.cluster_size;
  }
  agg.time = agg.time / num_trials;
  agg.symmetric_distance = agg.symmetric_distance / num_trials;
  agg.cluster_size = agg.cluster_size / num_trials;

  return agg;
}

/**
 * Given a results file and a single experimental result, write out the result.
 *
 * Does add a comma at the beginning.
 * Does not add a newline to the file.
 */
void write_result(std::ofstream* os, ExperimentalResult* res) {
  *os << ", " << res->time;
  *os << ", " << res->cluster_size;
  *os << ", " << res->symmetric_distance;
}

/**
 * Perform a comparison between on-disk and in-memory clustering for a given
 * number of clusters.
 *
 * Performs 10 trials for each algorithm and writes the results to the two
 * results files.
 *
 * Increments the run ID as it goes along.
 */
void compare_local_clustering(stag_int num_clusters,
                              std::ofstream* detail_file,
                              std::ofstream* aggregate_file,
                              stag_int& experiment_runid) {
  std::string adj_filename = "../data/sbm/k" + std::to_string(num_clusters) + ".al";

  // Begin by shuffling the adjacency. This makes the comparison more fair since
  // the target cluster is not all in the same part of the file on disk.
  std::string adj_shuffled_fname = "../data/sbm/k" + std::to_string(num_clusters) + "shuffled.al";
  shuffle_adjacencylist(adj_filename, adj_shuffled_fname);

  // Store the results of all trials for aggregation afterwards.
  std::vector<ExperimentalResult> disk_results;
  std::vector<ExperimentalResult> mem_results;

  stag_int num_trials = 10;

  for (auto i = 0; i < num_trials; i++) {
    stag_int start_v = 1000 * i;

    ExperimentalResult disk_res = evaluate_ondisk_clustering(adj_shuffled_fname,
                                                             start_v);
    ExperimentalResult mem_res = evaluate_inmemory_clustering(adj_shuffled_fname,
                                                              start_v);

    // Save the results
    disk_results.push_back(disk_res);
    mem_results.push_back(mem_res);

    // Write out the details results as we go along.
    *detail_file << experiment_runid;
    *detail_file << ", " << num_clusters;
    write_result(detail_file, &disk_res);
    write_result(detail_file, &mem_res);
    *detail_file << std::endl;
    experiment_runid++;
  }

  // Compute tha aggregate results and write them out
  ExperimentalResult disk_agg_res = compute_aggregate_result(disk_results);
  ExperimentalResult mem_agg_res = compute_aggregate_result(mem_results);
  *aggregate_file << num_clusters;
  write_result(aggregate_file, &disk_agg_res);
  write_result(aggregate_file, &mem_agg_res);
  *aggregate_file << std::endl;
}

void compare_all_graphs() {
  // Open the results files
  std::string detailed_fname = "../results/sbm/results.csv";
  std::string aggregate_fname = "../results/sbm/aggregate_results.csv";
  std::ofstream os_detail(detailed_fname);
  std::ofstream os_aggregate(aggregate_fname);

  // Write the header line to the output files
  os_detail << "id, k, disk_time, disk_size, disk_symdiff, mem_time, mem_size, mem_symdiff" << std::endl;
  os_aggregate << "k, disk_time, disk_size, disk_symdiff, mem_time, mem_size, mem_symdiff" << std::endl;

  std::vector<stag_int> orders_of_magnitude = {10, 100, 1000};
  std::vector<stag_int> ks;
  for (stag_int om : orders_of_magnitude) {
    for (stag_int i = 1; i < 10; i++) {
      ks.push_back(i * om);
    }
  }
  ks.push_back(10 * orders_of_magnitude.back());

  // Iterate through the SBM graphs in the data directory, and perform 10 trials
  // for each graph.
  stag_int run_id = 0;
  for (auto k : ks) {
    compare_local_clustering(k,
                             &os_detail,
                             &os_aggregate,
                             run_id);
  }

  os_detail.close();
  os_aggregate.close();
}
