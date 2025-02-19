#include "argparser.h"

#include <iostream>
#include <cstring>

#include "string_utils.h"

bool starts_with(const char* string, const char* string1) {
    return !strncmp(string, string1, strlen(string1));
}

int32_t get_char_index(const char* string, const char character) {
    size_t argument_length = strlen(string);
    for (size_t i = 0; i < argument_length; i++) {
        if (string[i] == '=') {
            return i;
        }
    }

    return -1;
}

const char* get_value_after_equals(const char* arg) {
    int32_t equals_index = get_char_index(arg, '=');
    if (equals_index == -1) {
        return nullptr;
    }

    return arg + equals_index + 1;
}

bool is_argument_name_equal(const char* string, const char* expected_name) {
    const char last_char = string[strlen(expected_name)];
    return starts_with(string, expected_name) && (last_char == '\0' || last_char == '=');
}

namespace ArgumentParser {

const char* get_argument_name(ArgumentBase* argument) {
    return (argument->get_name() != nullptr ? argument->get_name() : new char[] {argument->get_short_name()});
}

ArgParser::ArgParser(const char* name) {
    this->name = name;
    this->arguments = new std::vector<ArgumentBase*>();
}

ArgParser::~ArgParser() {
    delete this->arguments;
}

bool ArgParser::parse_positional_arguments(std::vector<std::string_view>& arguments) {
    std::vector<ArgumentBase*> positional_arguments;

    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (argument->is_positional()) {
            positional_arguments.push_back(argument);
        }
    }

    std::vector<ArgumentBase*> left_arguments;
    ArgumentBase* multi_value_argument = nullptr;
    std::vector<ArgumentBase*> right_arguments;

    size_t i = 0;
    while (i < positional_arguments.size()) {
        ArgumentBase* positional_argument = positional_arguments[i];
        if (positional_argument->is_multi_value()) {
            break;
        }
        
        left_arguments.push_back(positional_argument);
        i++;
    }

    if (i < positional_arguments.size()) {
        multi_value_argument = positional_arguments[i];
        ++i;
    }

    while (i < positional_arguments.size()) {
        ArgumentBase* positional_argument = positional_arguments[i];
        if (positional_argument->is_multi_value()) {
            return false;
        }
        
        right_arguments.push_back(positional_argument);
        i++;
    }

    if (arguments.size() < (left_arguments.size() + right_arguments.size())) {
        return false;
    }

    for (size_t i = 0; i < left_arguments.size(); ++i) {
        if (!left_arguments[i]->parse_value(arguments[i].data())) {
            return false;
        }
    }

    for (size_t i = 0; i < right_arguments.size(); ++i) {
        if (!right_arguments[right_arguments.size() - i - 1]->parse_value(arguments[arguments.size() - i - 1].data())) {
            return false;
        }
    }

    for (size_t i = left_arguments.size(); i < arguments.size() - right_arguments.size(); ++i) {
        if (!multi_value_argument->parse_value(arguments[i].data())) {
            return false;
        }
    }

    return true;
}

bool ArgParser::validate_arguments() {
    if (this->help_argument != nullptr && this->help_argument->get_value_unsafe()) {
        return true;
    }

    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (argument->is_multi_value()) {
            if (argument->get_value_count() < argument->get_min_value_count()) {
                std::cerr << "Parsing error: argument value count is less than required. Argument name: " << get_argument_name(argument) << '\n';
                return false;
            }
        } else if (argument->should_have_argument() && !argument->has_value() && !argument->has_default_value()) {
            std::cerr << "Parsing error: argument value not found. Argument name: " << get_argument_name(argument) << '\n';
            return false;
        }
    }

    return true;
}

bool ArgParser::handle_argument_value(ArgumentBase* argument, std::string_view arg, std::string_view next_arg) {
    const char* value = nullptr;
    if (argument->should_have_argument()) {
        value = get_value_after_equals(arg.data());
        if (value == nullptr) {
            if (!next_arg.size()) {
                std::cerr << "Missing expected argument: argument count is too low." << '\n';
                return false;
            }

            if (next_arg[0] == '-') {
                std::cerr << "Missing expected argument: next argument is not a value." << '\n';
                return false;
            }

            this->may_next_argument_be_free = false;
            value = next_arg.data();
        } 
        
        if (value == nullptr) {
            std::cerr << "Missing expected argument: argument value not found." << '\n';
            return false;
        }
    } else {
        this->may_next_argument_be_free = true;
    }

    argument->parse_value(value);
    return true;
}

ArgumentBase* ArgParser::find_argument_by_name(const char* argument_name) {    
    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (is_argument_name_equal(argument_name, argument->get_name()) || (argument_name[0] == argument->get_short_name())) {
            return argument;
        }
    }

    // TODO: handle a error or allow user to do it
    exit(EXIT_FAILURE);

    return nullptr;
}

ArgumentBase* ArgParser::find_argument_by_full_name(const char* argument_name) {    
    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (is_argument_name_equal(argument_name, argument->get_name())) {
            return argument;
        }
    }

    return nullptr;
}

ArgumentBase* ArgParser::find_argument_by_short_name(const char argument_name) {    
    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (argument_name == argument->get_short_name()) {
            return argument;
        }
    }

    return nullptr;
}

bool ArgParser::parse(int argc, char** argv) {
    std::vector<std::string_view> string_views;

    for (size_t i = 0; i < argc; i++) {
        string_views.emplace_back(argv[i]);
    }

    return this->parse(string_views);
}

bool ArgParser::parse(const std::vector<std::string>& args) {
    std::vector<std::string_view> string_views;

    for (auto& arg : args) {
        string_views.emplace_back(arg);
    }

    return this->parse(string_views);
}

bool ArgParser::parse_argument(std::string_view arg, std::string_view next_arg) {
    ArgumentBase* argument = this->find_argument_by_full_name(arg.substr(2).data());
    if (argument == nullptr) {
        return false;
    }

    return this->handle_argument_value(argument, arg, next_arg);
}

bool ArgParser::parse_short_argument(std::string_view arg, std::string_view next_arg) {
    size_t arg_length = arg.size();
    size_t equals_index = arg.find('=');
    if (equals_index != std::string_view::npos) {
        arg_length = equals_index;
    }
    
    for (size_t i = 1; i < arg_length; i++) {
        ArgumentBase* argument = this->find_argument_by_short_name(arg[i]);
        if (argument == nullptr) {
            return false;
        }
        
        if (argument == nullptr) {
            std::cerr << "Unknown short argument name: " << arg[i] << '\n';
            return false;
        }

        if (argument->should_have_argument() && arg_length > 2) {
            std::cerr << "An argument with a value cannot be merged with others.\n";
            return false;
        }
        
        if (!this->handle_argument_value(argument, arg, next_arg)) {
            return false;
        }
    }

    return true;
}

bool ArgParser::parse(const std::vector<std::string_view>& args) {
    this->may_next_argument_be_free = true;
    bool is_positional_only = false;

    std::vector<std::string_view> positional_arguments;

    for (size_t i = 1; i < args.size(); i++) {
        std::string_view arg = args[i];
        std::string_view next_arg;
        if (i + 1 < args.size()) {
            next_arg = args[i + 1];
        }

        if (is_positional_only) {
            positional_arguments.push_back(arg);
        } else if (arg.starts_with("--")) {
            if (arg.size() == 2) {
                is_positional_only = true;
                continue;
            }
            
            if (!parse_argument(arg, next_arg)) {
                return false;
            }
        } else if (arg.starts_with('-') && arg.size() != 1) {
            if (!parse_short_argument(arg, next_arg)) {
                return false;
            }
        } else {
            if (this->may_next_argument_be_free) {
                positional_arguments.push_back(arg);
            }

            this->may_next_argument_be_free = true;
        }
    }

    if (!this->parse_positional_arguments(positional_arguments)) {
        return false;
    }

    return this->validate_arguments();
}

void ArgParser::set_help_formatter(const AbstractHelpFormatter* formatter) {
    this->description_formatter = formatter;
}

void ArgParser::add_help(char short_argument_name, const char* argument_name, const char* program_description) {
    this->help_argument = &this->add_flag(short_argument_name, argument_name, "Display this help and exit");

    if (program_description != nullptr) {
        this->description = program_description;
    }    
}

bool ArgParser::help() {
    if (this->help_argument == nullptr) {
        return false;
    }

    return this->help_argument->get_value_unsafe();
}

std::string ArgParser::get_help_description() {
    return this->description_formatter->format(this->name, this->description, *this->arguments);
}

StringArgument& ArgParser::add_string_argument(const char* argument_name, const char* description) {
    return this->add_argument<StringArgument>(argument_name, description);
}

StringArgument& ArgParser::add_string_argument(char short_argument_name, const char* argument_name, const char* description) {
    return this->add_argument<StringArgument>(short_argument_name, argument_name, description);
}

std::string ArgParser::get_string_value(const char* argument_name) {
    return get_argument_value<std::string>(argument_name).value();
}

IntArgument& ArgParser::add_int_argument(const char* argument_name, const char* description) {
    return this->add_argument<IntArgument>(argument_name, description);
}

IntArgument& ArgParser::add_int_argument(char short_argument_name, const char* argument_name, const char* description) {
    return this->add_argument<IntArgument>(short_argument_name, argument_name, description);
}

int32_t ArgParser::get_int_value(const char* argument_name) {
    return get_argument_value<int32_t>(argument_name).value();
}

int32_t ArgParser::get_int_value(const char* argument_name, size_t index) {
    return get_argument_value<int32_t>(argument_name, index).value();
}

FlagArgument& ArgParser::add_flag(const char* argument_name, const char* description) {
    FlagArgument& argument = this->add_argument<FlagArgument>(argument_name, description);
    
    argument.set_should_have_argument(false);
    argument.set_default_value(false);

    return argument;
}

FlagArgument& ArgParser::add_flag(const char short_argument_name, const char* argument_name, const char* description) {
    FlagArgument& argument = this->add_argument<FlagArgument>(short_argument_name, argument_name, description);
    
    argument.set_should_have_argument(false);
    argument.set_default_value(false);

    return argument;
}

bool ArgParser::get_flag(const char* argument_name) {
    return get_argument_value<bool>(argument_name).value();
}

}