cmake_minimum_required(VERSION 3.22)
project(local_clustering_examples)

set(CMAKE_CXX_STANDARD 20)

# Find and include the STAG library
find_package(stag REQUIRED)
message(STATUS "Found STAG!")
include_directories(${STAG_INCLUDE_DIRS})

# Define the executable for the SBM demo
add_executable(sbm_example
        sbm_src/main.cpp sbm_src/generate.cpp sbm_src/experiments.cpp sbm_src/experiments.h sbm_src/generate.h)
target_link_libraries(sbm_example stag)

# Define the executable for the next demo
add_executable(wiki_example
        wiki_src/main.cpp wiki_src/wikidata.h wiki_src/experiments.cpp wiki_src/experiments.h wiki_src/wikidata.cpp)
target_link_libraries(wiki_example stag)
