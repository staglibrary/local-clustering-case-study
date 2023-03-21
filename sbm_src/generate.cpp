/**
* Methods for generating test graphs.
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <stag/graph.h>
#include <stag/random.h>
#include <stag/graphio.h>
#include <stag/utility.h>

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

void get_last_line_of_file(std::string& fname, std::string& line) {
  std::ifstream fin(fname);
  if(fin.is_open()) {
    fin.seekg(-1, std::ios_base::end);                // go to one spot before the EOF

    bool keepLooping = true;
    while(keepLooping) {
      char ch;
      fin.get(ch);                            // Get current byte's data

      if((int)fin.tellg() <= 1) {             // If the data was at or before the 0th byte
        fin.seekg(0);                       // The first line is the last line
        keepLooping = false;                // So stop there
      }
      else if(ch == '\n') {                   // If the data was a newline
        keepLooping = false;                // Stop at the current position.
      }
      else {                                  // If the data was neither a newline nor at the 0 byte
        fin.seekg(-2, std::ios_base::cur);        // Move to the front of that data, then to the front of the data before it
      }
    }

    getline(fin, line);                      // Read the current line

    // Close the output file.
    fin.close();
  }
}

/**
 * Convert an adjacency list to an edgelist with the given permutation of
 * node IDs.
 */
void adjacencylist_to_edgelist_with_perm(
    std::string &adjacencylist_fname, std::string &edgelist_fname,
    std::vector<stag_int>& perm) {
  // Open input and output streams
  std::ifstream is(adjacencylist_fname);
  std::ofstream os(edgelist_fname);
  if (!is.is_open()) throw std::runtime_error(std::strerror(errno));
  if (!os.is_open()) throw std::runtime_error(std::strerror(errno));

  // We will include any comments up until the first content line.
  // This preserves 'header' information as a comment.
  bool written_content = false;

  // Read the file in one line at a time
  std::string line;
  std::vector<stag::edge> neighbours;
  while (stag::safeGetline(is, line)) {
    if (line[0] != '#' && line[0] != '/' && line.length() > 0) {
      try {
        // This line of the input file isn't a comment, parse it.
        neighbours = stag::parse_adjacencylist_content_line(line);
        written_content = true;

        // Add the edges to the edgelist file
        for (auto this_edge : neighbours) {
          // Since the adjacency list contains edges in both direction, only
          // add them to the edgelist when v1 is less than v2
          if (this_edge.v1 <= this_edge.v2) {
            os << perm.at(this_edge.v1) << " " << perm.at(this_edge.v2) << " " << this_edge.weight << std::endl;
          }
        }
      } catch (std::invalid_argument &e) {
        // Re-throw any parsing errors
        throw(std::runtime_error(e.what()));
      }
    } else {
      if (!written_content) {
        os << line << std::endl;
      }
    }
  }

  // Close the file streams
  is.close();
  os.close();
}

/**
 * Get the maximum node ID in an adjacency list.
 * @param fname
 */
stag_int get_adjlist_max_nodeid(std::string& fname) {
  std::string last_line;
  get_last_line_of_file(fname, last_line);
  std::vector<stag::edge> edges = stag::parse_adjacencylist_content_line(last_line);
  if (edges.size() > 0) {
    return edges.at(0).v1;
  } else {
    return -1;
  }
}

/**
 * Given an adjacency list file, shuffle the node IDs
 * @param fname
 */
void shuffle_adjacencylist(std::string& fname, std::string& new_fname) {
  stag_int max_node_id = get_adjlist_max_nodeid(fname);

  // Begin by creating the mapping from old node IDs to new ones.
  std::vector<stag_int> permutation;
  for (stag_int i = 0; i <= max_node_id; i++) {
    permutation.push_back(i);
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(permutation.begin(), permutation.end(), gen);

  // Iterate through the adjacency list, creating an edgelist with the
  // permuted vertices.
  std::string temp_fname = tmpnam(nullptr);
  adjacencylist_to_edgelist_with_perm(fname, temp_fname, permutation);

  // Now convert the edgelist back into an adjacencylist and we're done.
  stag::edgelist_to_adjacencylist(temp_fname, new_fname);
}
