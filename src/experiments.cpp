#include <graph.h>
#include <cluster.h>
#include <iostream>

#include "wikidata.h"

#include "experiments.h"

stag_int symmetric_difference(std::vector<stag_int>& set1, std::vector<stag_int>& set2) {
  // Sort both inputs.
  std::sort(std::begin(set1), std::end(set1));
  std::sort(std::begin(set2), std::end(set2));

  // Compute the differences
  std::vector<stag_int> diff1;
  std::vector<stag_int> diff2;

  std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),
                      std::inserter(diff1, diff1.begin()));

  std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(),
                      std::inserter(diff2, diff2.begin()));

  return diff1.size() + diff2.size();
}

double prop_in(std::vector<stag_int>& set1, std::vector<stag_int> & set2) {
  // Sort both inputs.
  std::sort(std::begin(set1), std::end(set1));
  std::sort(std::begin(set2), std::end(set2));

  // Compute the differences
  std::vector<stag_int> diff1;

  std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),
                      std::inserter(diff1, diff1.begin()));

  return (double) diff1.size() / (double) set1.size();
}

void local_clustering_experiment(stag_int starting_vertex) {
  // Create a graph from the wikipedia data
  std::string wiki_filename = "data/wiki/wiki-topcats.al";
  stag::AdjacencyListLocalGraph wiki_graph(wiki_filename);

  // Find a local cluster with different sizes
  std::vector<double> vols = {100, 1000, 2000, 10000};
  for (double target_vol : vols) {
    std::string start_name = get_page_name(starting_vertex);
    std::cout << "Clustering from '" << start_name << "' with volume " << target_vol << std::endl;
    std::vector<stag_int> cluster = stag::local_cluster(&wiki_graph, starting_vertex,
                                                        target_vol);

    // Display the page names
    for (auto v : cluster) {
      std::cout << v << " " << get_page_name(v) << std::endl;
    }
    std::cout << std::endl;

    // Check the conductance
    double cond = stag::conductance(&wiki_graph, cluster);
    std::cout << "Conductance: " << cond << std::endl;
    std::cout << std::endl;
  }
}