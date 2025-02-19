#include "string_utils.h"
#include "string_utils.h"

// Inspired by this: https://stackoverflow.com/a/5289170/14915825
std::string join_strings(std::vector<std::string>& strings, const char* const delim) {
    std::ostringstream imploded;

    auto b = strings.begin();
    auto e = strings.end();

    if (b == e) {
        return *b;
    }
    
    std::copy(b, prev(e), std::ostream_iterator<std::string>(imploded, delim));
    b = prev(e);
    if (b != e) {
        imploded << *b;
    }
    
    return imploded.str();
}