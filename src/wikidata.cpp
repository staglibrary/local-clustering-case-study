#include <utility.h>
#include "wikidata.h"

stag_int goto_next_content_line(std::ifstream* ifs, std::streampos* eof) {
  std::string current_line;

  // Read the current line, discarding it. This leaves the file pointer at the
  // beginning of a line.
  std::streampos current_loc = ifs->tellg();
  if (current_loc != 0) stag::safeGetline(*ifs, current_line);

  current_loc = ifs->tellg();
  while (true) {
    // If the current position is the end of the file, we have failed to find
    // a content line. Return -1.
    if (current_loc == *eof) {
      return -1;
    }

    // Read until we find a non-empty line.
    // Make sure to set current_loc to the position just before calling getline.
    current_line.clear();
    while (current_line.empty()) {
      current_loc = ifs->tellg();
      stag::safeGetline(*ifs, current_line);
    }

    // Check if this line is a valid content line.
    size_t split_pos = current_line.find(' ');
    if (split_pos != std::string::npos) {
      std::string token = current_line.substr(0, split_pos);
      stag_int source_node_id = std::stoi(token);

      // We found a content line, reset the position of the reader to the start
      // of the line and return the node id.
      ifs->seekg(current_loc);
      return source_node_id;
    }
  }

  // If we couldn't find a content line, then the adjacencylist is badly formed.
  throw std::runtime_error("Malformed adjacencylist file.");
}

std::string get_page_name(stag_int v) {
  // We will find the page name in the wiki-topcats-page-names.txt file.
  std::string filename = "data/wiki/wiki-topcats-page-names.txt";
  std::ifstream ifs(filename);
  if (!ifs.is_open()) throw std::runtime_error("Couldn't open page name file.");

  // Set the maximum and minimum ranges of the file to search
  ifs.seekg(0, std::ios::end);
  std::streampos eof = ifs.tellg();

  std::streampos range_min = 0;
  std::streampos range_max = eof;

  // Perform a binary search for the target node
  stag_int current_id;
  bool found_target = false;
  while (!found_target) {
    // If min is greater than max, then we have failed to find our target point
    if (range_min > range_max) throw std::runtime_error("Couldn't find node in page name file.");

    // Search half-way between the search points.
    stag_int search_point = floor((range_max + range_min) / 2);

    // Go to the search point on disk
    ifs.seekg((std::streampos) search_point);
    current_id = goto_next_content_line(&ifs, &eof);

    if (current_id == v) {
      found_target = true;
    } else if (current_id == -1 || current_id > v) {
      range_max = search_point - std::streamoff(1);
    } else {
      range_min = search_point + std::streamoff(1);
    }
  }

  // We are now pointing at the start of the correct line. Parse it.
  std::string content_line;
  stag::safeGetline(ifs, content_line);
  size_t split_pos = content_line.find(' ');

  // Return everything after the first space
  content_line.erase(0, split_pos + 1);
  return content_line;
}

std::vector<stag_int> parse_category_line(std::string& input_line, std::string& cat_name) {
  size_t split_pos = input_line.find(';');
  cat_name = input_line.substr(0, split_pos);
  input_line.erase(0, split_pos + 1);

  // Find the nodes in this category
  std::vector<stag_int> nodes;
  std::string token;
  while ((split_pos = input_line.find(' ')) != std::string::npos) {
    token = input_line.substr(0, split_pos);
    input_line.erase(0, split_pos + 1);
    if (token.length() == 0) continue;

    nodes.push_back(std::stoi(token));
  }

  return nodes;
}

std::vector<stag_int> get_gt_category(std::string& category_name) {
  // Look through the category file until we find one matching the name
  std::string filename = "data/wiki/wiki-topcats-categories.txt";
  std::ifstream ifs(filename);
  if (!ifs.is_open()) throw std::runtime_error("Couldn't open category file.");

  std::string line;
  std::string line_cat_name;
  while (stag::safeGetline(ifs, line)) {
    std::vector<stag_int> nodes = parse_category_line(line, line_cat_name);

    if (line_cat_name == category_name) {
      // We have found the correct category
      return nodes;
    }
  }

  // We failed to find the category
  throw std::runtime_error("Couldn't find category.");
}
