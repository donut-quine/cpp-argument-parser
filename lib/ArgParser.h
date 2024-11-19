#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "argument.h"
#include "help_formatter.h"

namespace ArgumentParser {

const static AbstractHelpFormatter* DEFAULT_FORMATTER = new DefaultHelpFormatter();

class ArgParser {
private:
    const char* name = nullptr;
    const char* description = nullptr;

    const AbstractHelpFormatter* description_formatter = DEFAULT_FORMATTER;

    std::vector<ArgumentBase*>* arguments = nullptr;
    bool may_next_argument_be_free = false;
    ArgumentBase* positional_argument = nullptr;

    FlagArgument* help_argument = nullptr;

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

    bool parse(int argc, char** argv);

    bool parse(std::vector<std::string> args);

    void add_help(char short_argument_name, const char* argument_name, const char* description = nullptr);

    void set_help_formatter(const AbstractHelpFormatter* formatter);

    bool help();

    std::string get_help_description();

    template <typename T> T& add_argument(const char* argument_name, const char* description) {
        T* argument = new T(argument_name, description);
        this->arguments->push_back(argument);
        return *argument;
    }

    template <typename T> T& add_argument(char short_argument_name, const char* argument_name, const char* description) {
        T* argument = new T(short_argument_name, argument_name, description);
        this->arguments->push_back(argument);
        return *argument;
    }

    template <typename T> T& get_argument(const char* argument_name) {
        return *reinterpret_cast<T*>(this->find_argument_by_name(argument_name));
    }

    template <typename T> std::optional<T> get_argument_value(const char* argument_name) {
        ArgumentBase* argument = find_argument_by_name(argument_name);
        if (argument == nullptr) {
            return std::nullopt;
        }
        
        std::any value = argument->get_value();

        if (value.type() != typeid(T)) {
            return std::nullopt;
        }

        return std::any_cast<T>(value);
    }

    template <typename T> std::optional<T> get_argument_value(const char* argument_name, size_t index) {
        ArgumentBase* argument = find_argument_by_name(argument_name);
        if (argument == nullptr) {
            return std::nullopt;
        }
        
        std::any value = argument->get_value(index);

        if (value.type() != typeid(T)) {
            return std::nullopt;
        }

        return std::any_cast<T>(value);
    }

    StringArgument& add_string_argument(const char* argument_name, const char* description = nullptr);

    StringArgument& add_string_argument(char short_argument_name, const char* argument_name, const char* description = nullptr);

    std::string get_string_value(const char* argument_name);

    IntArgument& add_int_argument(const char* argument_name, const char* description = nullptr);

    IntArgument& add_int_argument(char short_argument_name, const char* argument_name, const char* description = nullptr);

    int32_t get_int_value(const char* argument_name);

    int32_t get_int_value(const char* argument_name, size_t index);

    FlagArgument& add_flag(const char* argument_name, const char* description = nullptr);

    FlagArgument& add_flag(char short_argument_name, const char* argument_name, const char* description = nullptr);

    bool get_flag(const char* argument_name);
};

} // namespace ArgumentParser