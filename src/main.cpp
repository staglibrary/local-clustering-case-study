#include <graph.h>
#include "experiments.h"

int main(int argc, char** args) {
  stag_int start_vertex = std::stoi(args[1]);
  local_clustering_experiment(start_vertex);
  return 0;
}