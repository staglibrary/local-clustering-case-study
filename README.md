# Local Clustering Case Studies
This repository contains several case studies demonstrating the local clustering
method of the STAG library.

These examples use the STAG library which should be installed for C++ and Python
by following the instructions in the
[STAG documentation](https://staglibrary.io/docs/cpp/).

## Stochastic Block Model Example
This example demonstrates the advantage of using the
[stag::AdjacencyListLocalGraph](https://staglibrary.io/docs/cpp/classstag_1_1AdjacencyListLocalGraph.html)
class to perform local clustering on a graph stored on disk.

When performing local clustering, it is not necessary to read the entire graph
into memory.
This allows for much faster local clustering with the AdjacencyListLocalGraph
when compared with the method which reads the entire graph.

### Preparing the data
Download the [sbm data](https://github.com/staglibrary/local-clustering-case-study/releases/download/v1.0/sbm.tgz)
to the data directory.
Then, from the data directory, extract the SBM data with the following command.

```bash
tar xzvf sbm.tgz
```

This will create an `sbm` directory containing the data.

### Compile the code
From the root directory of the repository, compile the SBM experiment code with
the following commands.

```bash
cmake -S . -B sbm_build
cmake --build sbm_build --target sbm_example
```

### Run the experiment
Then, you can run the SBM experiment from the build directory.

```bash
cd sbm_build
./sbm_example
```

This will perform local clustering on a series of graphs with increasing size
and report the running time for
* local clustering using `stag::AdjacencyListLocalGraph`, which doesn't read the 
graph into memory; and
* local clustering using `stag::Graph`, which does read the graph into memory.

## Wikipedia Example

### Preparing the data
Download the [wikipedia data](https://github.com/staglibrary/local-clustering-case-study/releases/download/v1.0/wiki.tgz) to the data directory.
Then, from the data directory, extract the wikipedia data with the following command.

```bash
tar xzvf wiki.tgz
```

This will create a `wiki` directory containing the data.
The data is also available from [here](http://snap.stanford.edu/data/wiki-topcats.html).

If you download the data from SNAP, you should convert the edgelist file to
an adjacency list, for example by using the `stag_edge2adj` tool provided with
the STAG C++ library.

```bash
stag_edge2adj wiki-topcats.txt wiki-topcats.al
```

### Compiling the code
From the root directory of the repository, compile the wikipedia experiment code with
the following commands.

```bash
cmake -S . -B wiki_build
cmake --build wiki_build --target wiki_example
```

### Running the experiment
From the `wiki_example` directory, run the `wiki_example` program which takes
two arguments:

* the node ID of a vertex in the wikipedia graph; and
* the target volume of the cluster to return.

For example:
```bash
./wiki_example 1727023 2000
./wiki_example 1680321 2000
./wiki_example 1715407 2000
```

The program will print the names of the nodes in the graphs found by the
local clustering algorithm.

Note that this example uses the `AdjacencyListLocalGraph` class, and so it
doesn't need to read the complete wikipedia graph into memory in order
to return a local cluster.

## Neo4j Example

The third example is to 
