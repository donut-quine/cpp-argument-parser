#include "ArgParser.h"

#include <iostream>

#include "argument_parser.h"

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

class FlagArgumentParser : public AbstractArgumentParser<bool> {
public:
    bool parse_value(const char* string, bool* default_value) const {
        return !*default_value;
    }
};

class IntArgumentParser : public AbstractArgumentParser<int32_t> {
public:
    int32_t parse_value(const char* string, int32_t* default_value) const {
        return std::stoi(string);
    }
};

class StringArgumentParser : public AbstractArgumentParser<std::string> {
public:
    std::string parse_value(const char* string, std::string* default_value) const {
        return std::string(string);
    }
};

const static FlagArgumentParser* flag_parser = new FlagArgumentParser();
const static IntArgumentParser* int_parser = new IntArgumentParser();
const static StringArgumentParser* string_parser = new StringArgumentParser();

ArgParser::ArgParser(const char* name) {
    this->name = name;
    this->arguments = new std::vector<ArgumentBase*>();
}

ArgParser::~ArgParser() {
    delete this->arguments;
}

void ArgParser::resolve_positional_argument() {
    ArgumentBase* positional_argument = nullptr;

    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (argument->is_positional()) {
            if (positional_argument != nullptr) {
                std::cerr << "There is more than 1 positional argument";
                exit(EXIT_FAILURE);
            }

            positional_argument = argument;
        }
    }

    this->positional_argument = positional_argument;
}

bool ArgParser::validate_arguments() {
    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (argument->is_multi_value()) {
            if (argument->get_value_count() < argument->get_min_value_count()) {
                return false;
            }
        }
    }

    return true;
}

void ArgParser::handle_argument_value(ArgumentBase* argument, const char* arg, const char* next_arg) {
    const char* value = nullptr;
    if (argument->should_have_argument()) {
        value = get_value_after_equals(arg);
        if (value == nullptr) {
            if (next_arg == nullptr) {
                std::cerr << "Missing expected argument: argument count is too low.";
                exit(EXIT_FAILURE);
            }

            if (next_arg[0] == '-') {
                std::cerr << "Missing expected argument: next argument is not a value.";
                exit(EXIT_FAILURE);
            }

            this->may_next_argument_be_free = false;
            value = next_arg;
        } 
        
        if (value == nullptr) {
            std::cerr << "Missing expected argument: argument value not found.";
            exit(EXIT_FAILURE);
        }
    } else {
        this->may_next_argument_be_free = true;
    }

    argument->parse_value(value);
}

bool ArgParser::parse_single_argument(const char* arg, const char* next_arg) {
    if (arg[0] != '-') {
        if (this->may_next_argument_be_free && this->positional_argument != nullptr) {
            this->positional_argument->parse_value(arg);
        }

        this->may_next_argument_be_free = true;
        return true;
    }

    ArgumentBase* argument = nullptr;
    if (arg[1] == '-') {
        argument = this->find_argument_by_full_name(arg + 2);
        if (argument == nullptr) {
            return false;
        }

        this->handle_argument_value(argument, arg, next_arg);
    } else {
        size_t arg_length = strlen(arg);
        int32_t equals_index = get_char_index(arg, '=');
        if (equals_index != -1) {
            arg_length = equals_index;
        }
        
        for (size_t i = 1; i < arg_length; i++) {
            argument = this->find_argument_by_short_name(arg[i]);
            if (argument == nullptr) {
                std::cerr << "Unknown short argument name: " << arg[i] << '\n';
                return false;
            }

            if (argument->should_have_argument() && arg_length > 2) {
                std::cerr << "An argument with a value cannot be merged with others.\n";
                return false;
            }
            
            this->handle_argument_value(argument, arg, next_arg);
        }
    }
    
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

bool ArgParser::Parse(int argc, char** argv) {
    this->may_next_argument_be_free = true;

    resolve_positional_argument();

    for (size_t i = 1; i < argc; i++) {
        const char* arg = argv[i];
        const char* next_arg = nullptr;
        if (i + 1 < argc) {
            next_arg = argv[i + 1];
        }

        parse_single_argument(arg, next_arg);
    }

    return this->validate_arguments();
}

bool ArgParser::Parse(std::vector<std::string> args) {
    this->may_next_argument_be_free = true;

    this->resolve_positional_argument();

    for (size_t i = 1; i < args.size(); i++) {
        const char* arg = args[i].c_str();
        const char* next_arg = nullptr;
        if (i + 1 < args.size()) {
            next_arg = args[i + 1].c_str();
        }

        this->parse_single_argument(arg, next_arg);
    }

    return this->validate_arguments();
}

void ArgParser::AddHelp(char short_argument_name, const char* argument_name, const char* description) {
    this->help_argument = &this->AddFlag(short_argument_name, argument_name, description);
}

bool ArgParser::Help() {
    if (this->help_argument == nullptr) {
        return false;
    }

    return this->help_argument->GetValue();
}

const char* ArgParser::HelpDescription() {
    return nullptr;
}

Argument<std::string>& ArgParser::AddStringArgument(const char* argument_name, const char* description) {
    Argument<std::string>* argument = new Argument<std::string>(string_parser, argument_name, description);
    this->arguments->push_back(argument);
    return *argument;
}

Argument<std::string>& ArgParser::AddStringArgument(char short_argument_name, const char* argument_name, const char* description) {
    Argument<std::string>* argument = new Argument<std::string>(string_parser, short_argument_name, argument_name, description);
    this->arguments->push_back(argument);
    return *argument;
}

std::string ArgParser::GetStringValue(const char* argument_name) {
    ArgumentBase* argument = this->find_argument_by_name(argument_name);

    return reinterpret_cast<Argument<std::string>*>(argument)->GetValue();
}

Argument<int32_t>& ArgParser::AddIntArgument(const char* argument_name, const char* description) {
    Argument<int32_t>* argument = new Argument<int32_t>(int_parser, argument_name, description);
    this->arguments->push_back(argument);
    return *argument;
}

Argument<int32_t>& ArgParser::AddIntArgument(char short_argument_name, const char* argument_name, const char* description) {
    Argument<int32_t>* argument = new Argument<int32_t>(int_parser, short_argument_name, argument_name, description);
    this->arguments->push_back(argument);
    return *argument;
}

int32_t ArgParser::GetIntValue(const char* argument_name) {
    ArgumentBase* argument = this->find_argument_by_name(argument_name);

    return reinterpret_cast<Argument<int32_t>*>(argument)->GetValue();
}

int32_t ArgParser::GetIntValue(const char* argument_name, size_t index) {
    ArgumentBase* argument = this->find_argument_by_name(argument_name);

    return reinterpret_cast<Argument<int32_t>*>(argument)->GetValue(index);
}

Argument<bool>& ArgParser::AddFlag(const char* argument_name, const char* description) {
    Argument<bool>* argument = new Argument<bool>(flag_parser, argument_name, description);
    argument->set_should_have_argument(false);
    argument->Default(false);

    this->arguments->push_back(argument);
    return *argument;
}

Argument<bool>& ArgParser::AddFlag(const char short_argument_name, const char* argument_name, const char* description) {
    Argument<bool>* argument = new Argument<bool>(flag_parser, short_argument_name, argument_name, description);
    argument->set_should_have_argument(false);
    argument->Default(false);

    this->arguments->push_back(argument);
    return *argument;
}

bool ArgParser::GetFlag(const char* argument_name) {
    ArgumentBase* argument = this->find_argument_by_name(argument_name);

    return reinterpret_cast<Argument<bool>*>(argument)->GetValue();
}

}