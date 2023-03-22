#include <graph.h>
#include "experiments.h"

int main(int argc, char** args) {
  stag_int start_vertex = std::stoi(args[1]);
  double target_vol = std::stod(args[2]);
  local_clustering_experiment(start_vertex, target_vol);
  return 0;
}