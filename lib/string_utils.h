#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <sstream>

// Copied from https://stackoverflow.com/a/5689061/14915825
std::string join_strings(std::vector<std::string>& strings, const char* const delim);