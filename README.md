# Local Clustering Case Studies
This repository contains several case studies demonstrating the local clustering
method of the STAG library.

These examples use the STAG library which should be installed for C++ and Python
by following the instructions in the
[STAG documentation](https://staglibrary.io/docs/cpp/).

## Stochastic Block Model Example



## Wikipedia Example

### Preparing the data
In the data directory, extract the wikipedia data with the following command.

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

### Running the experiment

```bash
./wiki_example 1727023 2000
```

```bash
./wiki_example 1680321 2000
```

```bash
./wiki_example 1715407 2000
```

## Neo4j Example

The third example is to 
