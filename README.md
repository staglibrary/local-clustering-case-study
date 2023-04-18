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
In this example, we will demonstrate the applicability of local clustering to
a real-world dataset, namely the wikipedia graph.

### Preparing the data
Download the [wikipedia data](https://github.com/staglibrary/local-clustering-case-study/releases/download/v1.0/wiki.tgz) to the data directory.
Then, from the data directory, extract the wikipedia data with the following command.

```bash
tar xzvf wiki.tgz
```

This will create a `wiki` directory containing the data.
The data is also available from [here](http://snap.stanford.edu/data/wiki-topcats.html).

If you download the data from SNAP, you should convert the edgelist file to
an adjacency list, for example by using the `stag_edge2adj` tool [provided with the STAG C++ library](https://staglibrary.io/docs/cpp/stag-tools.html).

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

### Running the example
From the `wiki_build` directory, run the `wiki_example` program which takes
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
Notice that the returned pages represent pages which are closely related to the
starting page.

Note that this example uses the `AdjacencyListLocalGraph` class, and so it
doesn't need to read the complete wikipedia graph into memory in order
to return a local cluster.

## Neo4j Example

The third example demonstrates the `stag.neo4j.Neo4jGraph` class provided by the
STAG Python library.
The `Neo4jGraph` object provides an interface to a graph stored in a
[Neo4j](https://neo4j.com/) graph database, either locally or in the cloud.
Then, the `stag.cluster.local_cluster` method can be used to search for related
nodes in the database.

### Creating a Neo4j database
From the [Neo4j](https://neo4j.com/) website, follow the prompts to create a free
example database in the AuraDB cloud.
You should use the default 'Movies' database.
Take a note of the following information when you create the new database.

* The generated password for accessing the database.
* The connection URI.

### Preparing your environment
Make sure that the STAG python library is installed.

```bash
python -m pip install stag
```

Add the Neo4j database details in the following environment variables.

* NEO4J_URI
* NEO4J_USER
* NEO4J_PASSWORD

For example:

```bash
export NEO4J_URI="neo4j+s://<uri>:7687"
export NEO4J_USER="neo4j"
export NEO4J_PASSWORD="<password>"
```

These environment variables will be used by the example script to connect to the 
database.

### Running the example
From the `neo4j_src` directory, run the example script with

```bash
python neo4j_movies.py <movie name>
```

For example, you could try the following movies.

* "The Matrix"
* "You've Got Mail"
* "Top Gun"
* "A Few Good Men"

You can also experiment with the size of the returned cluster by passing the 
`--volume` argument to the script. 

```bash
python neo4j_movies.py --volume 50 "The Matrix"
```
