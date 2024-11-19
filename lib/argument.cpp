#include "argument.h"

namespace ArgumentParser {

ArgumentBase::ArgumentBase(const char* name, const char* description) {
    this->name = name;
    this->description = description;
}

ArgumentBase::ArgumentBase(const char short_name, const char* name, const char* description) {
    this->short_name = short_name;
    this->name = name;
    this->description = description;
}

const char* ArgumentBase::get_name() {
    return this->name;
}

const char ArgumentBase::get_short_name() {
    return this->short_name;
}

const char* ArgumentBase::get_description() {
    return this->description;
}

std::optional<std::string> parse_string(const char* string_value, std::optional<std::string> default_value) {
    if (string_value == nullptr) {
        return std::nullopt;
    }
    
    return std::string(string_value);
}

std::optional<bool> parse_flag(const char* string_value, std::optional<bool> default_value) {
    if (default_value.has_value()) {
        return !default_value.value();
    }
    
    return std::nullopt;
}

} // namespace ArgumentParser
