#include "ArgParser.h"

#include <iostream>

bool starts_with(const char* string, const char* string1) {
    return !strncmp(string, string1, strlen(string1));
}

const char* get_value_after_equals(const char* arg) {
    size_t argument_length = strlen(arg);
    for (size_t i = 0; i < argument_length; i++) {
        if (arg[i] == '=') {
            return arg + i + 1;
        }
    }

    return nullptr;
}

bool is_argument_name_equal(const char* string, const char* expected_name) {
    const char last_char = string[strlen(expected_name)];
    return starts_with(string, expected_name) && (last_char == '\0' || last_char == '=');
}

bool is_argument_name_equal(const char* string, const char expected_name) {
    const char last_char = string[1];
    return string[0] == expected_name && (last_char == '\0' || last_char == '=');
}

namespace ArgumentParser {

class FlagArgumentParser : public ArgumentParser<bool> {
    virtual bool parse_value(const char* string) const override {
        return strcmp(string, "false");
    }
};

class IntArgumentParser : public ArgumentParser<int32_t> {
    virtual int32_t parse_value(const char* string) const override {
        return std::stoi(string);
    }
};

class StringArgumentParser : public ArgumentParser<std::string> {
    virtual std::string parse_value(const char* string) const override {
        return std::string(string);
    }
};

const static FlagArgumentParser* flag_parser = new FlagArgumentParser();
const static IntArgumentParser* int_parser = new IntArgumentParser();
const static StringArgumentParser* string_parser = new StringArgumentParser();

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

template <typename T> Argument<T>::Argument(const ArgumentParser<T>* parser, const char* name, const char* description) : ArgumentBase(name, description) {
    this->parser = parser;
}

template <typename T> Argument<T>::Argument(const ArgumentParser<T>* parser, const char short_name, const char* name, const char* description) : ArgumentBase(short_name, name, description) {
    this->parser = parser;
}

template <typename T> void Argument<T>::parse_value(const char* string_value) {
    T value = this->parser->parse_value(string_value);
    if (this->is_multi_value) {
        this->values->push_back(value);
        return;
    }
    
    *this->value = value;
}

template <typename T> bool Argument<T>::should_have_argument() {
    return this->_should_have_argument;
}

template <typename T> void Argument<T>::set_should_have_argument(bool value) {
    this->_should_have_argument = value;
}

template <typename T> void Argument<T>::Default(T value) {
    this->default_value = value;
}

template <typename T> T Argument<T>::GetDefault() {
    return this->default_value;
}

template <typename T> void Argument<T>::StoreValue(T& value) {
    this->value = value;
}

template <typename T> void Argument<T>::StoreValues(std::vector<T>& values) {
    this->values = values;
}

template <typename T> T Argument<T>::GetValue() {
    return *this->value;
}

template <typename T> T Argument<T>::GetValue(size_t index) {
    return (*this->values)[index];
}

template <typename T> Argument<T> Argument<T>::MultiValue(size_t min_argument_count) {
    this->is_multi_value = true;
    this->min_argument_count = min_argument_count;
    return this;
}

template <typename T> Argument<T> Argument<T>::Positional() {
    this->is_positional = true;
    return this;
}

ArgParser::ArgParser(const char* name) {
    this->name = name;
}

bool ArgParser::parse_single_argument(const char* arg, const char* next_arg, bool may_next_argument_be_free) {
    if (arg[0] != '-') {
        if (may_next_argument_be_free) {
            this->positional_arguments->push_back(arg);
        }

        return true;
    }

    for (size_t j = 0; j < this->arguments->size(); j++) {
        ArgumentBase* argument = (*this->arguments)[j];

        if ((arg[1] != '-' || !is_argument_name_equal(arg + 2, argument->get_name())) && !is_argument_name_equal(arg + 1, argument->get_short_name())) {
            continue;
        }

        const char* value = get_value_after_equals(arg);
        if (argument->should_have_argument() && value == nullptr) {
            if (next_arg == nullptr) {
                std::cerr << "Missing expected argument: argument count is too low.";
                exit(EXIT_FAILURE);
            }

            if (next_arg[0] == '-') {
                std::cerr << "Missing expected argument: next argument is not a value.";
                exit(EXIT_FAILURE);
            }

            may_next_argument_be_free = false;
            value = next_arg;
        } else {
            value = nullptr;
        }

        argument->parse_value(value);
    }

    return may_next_argument_be_free;
}

ArgumentBase* ArgParser::find_argument_by_name(const char* argument_name) {    
    for (size_t i = 0; i < this->arguments->size(); i++) {
        ArgumentBase* argument = (*this->arguments)[i];
        if (is_argument_name_equal(argument_name, argument->get_name()) || is_argument_name_equal(argument_name, argument->get_short_name())) {
            return argument;
        }
    }

    // TODO: handle a error or allow user to do it
    exit(EXIT_FAILURE);

    return nullptr;
}

bool ArgParser::Parse(int argc, char** argv) {
    bool may_next_argument_be_free = true;

    for (size_t i = 1; i < argc; i++) {
        const char* arg = argv[i];
        const char* next_arg = nullptr;
        if (i + 1 < argc) {
            next_arg = argv[i + 1];
        }        

        may_next_argument_be_free = parse_single_argument(arg, next_arg, may_next_argument_be_free);
    }

    return true;
}

bool ArgParser::Parse(std::vector<std::string> args) {
    bool may_next_argument_be_free = true;

    for (size_t i = 1; i < args.size(); i++) {
        const char* arg = args[i].c_str();
        const char* next_arg = nullptr;
        if (i + 1 < args.size()) {
            next_arg = args[i + 1].c_str();
        }        

        may_next_argument_be_free = parse_single_argument(arg, next_arg, may_next_argument_be_free);
    }

    return true;
}

void ArgParser::AddHelp(char short_argument_name, const char* argument_name, const char* description) {

}

bool ArgParser::Help() {
    return false;
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
    this->arguments->push_back(argument);
    return *argument;
}

Argument<bool>& ArgParser::AddFlag(char short_argument_name, const char* argument_name, const char* description) {
    Argument<bool>* argument = new Argument<bool>(flag_parser, short_argument_name, argument_name, description);
    this->arguments->push_back(argument);
    return *argument;
}

bool ArgParser::GetFlag(const char* argument_name) {
    ArgumentBase* argument = this->find_argument_by_name(argument_name);

    return reinterpret_cast<Argument<bool>*>(argument)->GetValue();
}

}