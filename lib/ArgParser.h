#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace ArgumentParser {

template <typename T> class Argument {
private:
public:
    void Default(T default_value);

    void StoreValue(T value);

    // TODO: set multi value flag and return this
    Argument<T> MultiValue(size_t min_argument_count = 0);

    // TODO: set positional flag and return this
    Argument<T> Positional();

    void StoreValues(std::vector<T> values);
};

class ArgParser {
private:
public:
    ArgParser(const char* name);

    bool Parse(int argc, char** argv);

    bool Parse(std::vector<std::string> args);

    void AddHelp(char short_argument_name, const char* argument_name, const char* description = nullptr);

    bool Help();

    const char* HelpDescription();

    Argument<std::string> AddStringArgument(const char* argument_name);

    Argument<std::string> AddStringArgument(char short_argument_name, const char* argument_name, const char* description = nullptr);

    std::string GetStringValue(const char* argument_name);

    Argument<int32_t> AddIntArgument(const char* argument_name, const char* description = nullptr);

    Argument<int32_t> AddIntArgument(char short_argument_name, const char* argument_name, const char* description = nullptr);

    int32_t GetIntValue(const char* argument_name);

    int32_t GetIntValue(const char* argument_name, int32_t index);

    Argument<bool> AddFlag(const char* argument_name, const char* description = nullptr);

    Argument<bool> AddFlag(char short_argument_name, const char* argument_name, const char* description = nullptr);

    bool GetFlag(const char* argument_name);
};

} // namespace ArgumentParser