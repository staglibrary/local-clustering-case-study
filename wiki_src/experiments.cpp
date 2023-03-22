#include <graph.h>
#include <cluster.h>
#include <iostream>

#include "wikidata.h"

#include "experiments.h"

void local_clustering_experiment(stag_int starting_vertex, double target_vol) {
  // Create a graph from the wikipedia data
  std::string wiki_filename = "../data/wiki/wiki-topcats.al";
  stag::AdjacencyListLocalGraph wiki_graph(wiki_filename);

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