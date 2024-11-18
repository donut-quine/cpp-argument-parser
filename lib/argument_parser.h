#pragma once

namespace ArgumentParser {

template <typename T>
class AbstractArgumentParser {
public:
    virtual T parse_value(const char* string, T* default_value) const = 0;
};

}
