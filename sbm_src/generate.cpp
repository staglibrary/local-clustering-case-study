/**
* Methods for generating test graphs.
*/
#include <iostream>
#include <stag/graph.h>
#include <stag/random.h>
#include <stag/graphio.h>

#include "generate.h"

void create_sbm_graphs() {
  // We will create a sequence of graphs. Each graph has some number of clusters k
  // We will set
  //   p = 0.01
  //   q = 0.001 / k
  //   n = 1000 * k
  //
  // Which results in a graph with k local clusters each of conductance about
  // 0.1.
  //
  // The degree of each node is about 10.

  // First create a vector of all the ks we'd like to generate.
  std::vector<stag_int> orders_of_magnitude = {10, 100, 1000, 10000};
  std::vector<stag_int> ks;
  for (stag_int om : orders_of_magnitude) {
    for (stag_int i = 1; i < 10; i++) {
      ks.push_back(i * om);
    }
  }
  ks.push_back(10 * orders_of_magnitude.back());

  // Now generate the graphs
  for (stag_int k : ks) {
    stag_int cluster_size = 1000;
    double p = 0.01;
    double q = 0.001 / (double) k;

    // Create the general sbm parameters
    std::vector<stag_int> cluster_sizes;
    DenseMat probabilities(k, k);
    for (auto i = 0; i < k; i++) {
      cluster_sizes.push_back(cluster_size);
      probabilities(i, i) = p;

      for (auto j = i + 1; j < k; j++) {
        probabilities(i, j) = q;
        probabilities(j, i) = q;
      }
    }

    std::cout << std::endl << "k = " << k << std::endl;
    std::cout << "Generating edgelist..." << std::endl;
    std::string edge_filename = "k" + std::to_string(k) + ".el";
    stag::general_sbm_edgelist(edge_filename, cluster_sizes, probabilities);

    std::cout << "Converting to adjacency list..." << std::endl;
    std::string adj_filename = "k" + std::to_string(k) + ".al";
    stag::edgelist_to_adjacencylist(edge_filename, adj_filename);
  }
}
