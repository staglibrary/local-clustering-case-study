//
// Created by macgr on 20/03/2023.
//

#ifndef LOCAL_CLUSTERING_EXAMPLES_WIKIDATA_H
#define LOCAL_CLUSTERING_EXAMPLES_WIKIDATA_H

#include <string>
#include <graph.h>


/**
 * Look up the name of the wikipedia page with the given ID.
 */
std::string get_page_name(stag_int v);


/**
 * Given the name of a wikipedia category, find the nodes which are in the
 * category.
 */
std::vector<stag_int> get_gt_category(std::string& category_name);

#endif //LOCAL_CLUSTERING_EXAMPLES_WIKIDATA_H
