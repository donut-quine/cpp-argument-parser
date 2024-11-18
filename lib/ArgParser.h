#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "argument.h"

namespace ArgumentParser {

class ArgParser {
private:
    const char* name;

    std::vector<ArgumentBase*>* arguments = nullptr;
    bool may_next_argument_be_free = false;
    ArgumentBase* positional_argument = nullptr;
    Argument<bool>* help_argument = nullptr;

    void resolve_positional_argument();

    bool validate_arguments();

    bool parse_single_argument(const char* arg, const char* next_arg);

    bool handle_argument_value(ArgumentBase* argument, const char* arg, const char* next_arg);

    ArgumentBase* find_argument_by_name(const char* argument_name);

    ArgumentBase* find_argument_by_full_name(const char* argument_name);

    ArgumentBase* find_argument_by_short_name(const char argument_name);
public:
    ArgParser(const char* name);

    ~ArgParser();

    bool Parse(int argc, char** argv);

    bool Parse(std::vector<std::string> args);

    void AddHelp(char short_argument_name, const char* argument_name, const char* description = nullptr);

    bool Help();

    const char* HelpDescription();

    Argument<std::string>& AddStringArgument(const char* argument_name, const char* description = nullptr);

    Argument<std::string>& AddStringArgument(char short_argument_name, const char* argument_name, const char* description = nullptr);

    std::string GetStringValue(const char* argument_name);

    Argument<int32_t>& AddIntArgument(const char* argument_name, const char* description = nullptr);

    Argument<int32_t>& AddIntArgument(char short_argument_name, const char* argument_name, const char* description = nullptr);

    int32_t GetIntValue(const char* argument_name);

    int32_t GetIntValue(const char* argument_name, size_t index);

    Argument<bool>& AddFlag(const char* argument_name, const char* description = nullptr);

    Argument<bool>& AddFlag(char short_argument_name, const char* argument_name, const char* description = nullptr);

    bool GetFlag(const char* argument_name);
};

} // namespace ArgumentParser