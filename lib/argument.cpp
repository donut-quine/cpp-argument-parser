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

} // namespace ArgumentParser
