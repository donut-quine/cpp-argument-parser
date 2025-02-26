#pragma once

#include <vector>
#include <string_view>
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

    FlagArgument* help_argument = nullptr;

    bool parse_positional_arguments(std::vector<std::string_view>& arguments);

    bool parse_argument(const std::string_view& arg, const std::string_view& next_arg);

    bool parse_short_argument(const std::string_view& arg, const std::string_view& next_arg);

    bool handle_argument_value(ArgumentBase* argument, const std::string_view& arg, const std::string_view& next_arg);

    bool validate_arguments();

    ArgumentBase* find_argument_by_name(const char* argument_name);

    ArgumentBase* find_argument_by_full_name(const char* argument_name);

    ArgumentBase* find_argument_by_short_name(const char argument_name);
public:
    ArgParser(const char* name);

    ~ArgParser();

    bool parse(int argc, const char** argv);

    bool parse(const std::vector<std::string>& args);

    bool parse(const std::vector<std::string_view>& args);

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

    FlagArgument& add_flag(const char* argument_name, const char* description = nullptr);

    FlagArgument& add_flag(char short_argument_name, const char* argument_name, const char* description = nullptr);

    bool get_flag(const char* argument_name);

#define CREATE_ARGUMENT_HEADER_FUNCTIONS(argument_type, value_type, type_name) \
    argument_type& add_##type_name##_argument(const char* argument_name, const char* description = nullptr); \
    \
    argument_type& add_##type_name##_argument(char short_argument_name, const char* argument_name, const char* description = nullptr); \
    \
    value_type get_##type_name##_value(const char* argument_name); \
    \
    value_type get_##type_name##_value(const char* argument_name, size_t index);

    CREATE_ARGUMENT_HEADER_FUNCTIONS(StringArgument, std::string, string);
    CREATE_ARGUMENT_HEADER_FUNCTIONS(IntArgument, int, int);
    CREATE_ARGUMENT_HEADER_FUNCTIONS(Int8Argument, int8_t, int8);
    CREATE_ARGUMENT_HEADER_FUNCTIONS(UInt8Argument, uint8_t, uint8);
    CREATE_ARGUMENT_HEADER_FUNCTIONS(Int16Argument, int16_t, int16);
    CREATE_ARGUMENT_HEADER_FUNCTIONS(UInt16Argument, uint16_t, uint16);
    CREATE_ARGUMENT_HEADER_FUNCTIONS(Int32Argument, int32_t, int32);
    CREATE_ARGUMENT_HEADER_FUNCTIONS(UInt32Argument, uint32_t, uint32);
};

} // namespace ArgumentParser