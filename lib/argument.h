#pragma once

#include <iostream>
#include <vector>

#include "argument_parser.h"
#include "string_utils.h"

namespace ArgumentParser {

class ArgumentBase {
private:
    char short_name = 0;
    const char* name = nullptr;
    const char* description = nullptr;
public:
    ArgumentBase(const char* name, const char* description = nullptr) {
        this->name = name;
        this->description = description;
    }

    ArgumentBase(const char short_name, const char* name, const char* description = nullptr) {
        this->short_name = short_name;
        this->name = name;
        this->description = description;
    }

    const char* get_name() {
        return this->name;
    }

    const char get_short_name() {
        return this->short_name;
    }

    const char* get_description() {
        return this->description;
    }
    
    virtual void parse_value(const char* string_value) = 0;

    virtual bool should_have_argument() = 0;

    virtual bool has_default_value() = 0;

    virtual bool has_value() = 0;

    virtual bool is_positional() = 0;

    virtual bool is_multi_value() = 0;

    virtual size_t get_min_value_count() = 0;

    virtual size_t get_value_count() = 0;
};

template <typename T> class Argument : public ArgumentBase {
private:
    const AbstractArgumentParser<T>* parser = nullptr;

    T* default_value = nullptr;
    T* value = nullptr;
    
    std::vector<T>* values = nullptr;

    bool _should_have_argument = true;
    bool _is_positional = false;

    bool _is_multi_value = false;
    size_t min_argument_count = 0;
public:
    Argument(const AbstractArgumentParser<T>* parser, const char* name, const char* description = nullptr) : ArgumentBase(name, description) {
        this->parser = parser;
    }

    Argument(const AbstractArgumentParser<T>* parser, const char short_name, const char* name, const char* description = nullptr) : ArgumentBase(short_name, name, description) {
        this->parser = parser;
    }
    
    void parse_value(const char* string_value) override {
        T value = this->parser->parse_value(string_value, this->default_value);
        if (this->_is_multi_value) {
            this->values->push_back(value);
            return;
        }
        
        set_value(value);
    }

    void set_value(T value) {
        if (this->value == nullptr) {
            this->value = new T(value);
        } else {
            *this->value = value;
        }
    }

    bool should_have_argument() override {
        return this->_should_have_argument;
    }

    bool has_default_value() override {
        return this->default_value != nullptr;
    }

    bool has_value() override {
        return this->value != nullptr || this->has_default_value();
    }

    bool is_positional() override {
        return this->_is_positional;
    }

    bool is_multi_value() override {
        return this->_is_multi_value;
    }

    size_t get_min_value_count() override {
        return this->min_argument_count;
    }

    size_t get_value_count() override {
        if (this->values == nullptr) {
            return 0;
        }
        
        return this->values->size();
    }

    void set_should_have_argument(bool value) {
        this->_should_have_argument = value;
    }

    void set_default_value(T value) {
        this->default_value = new T(value);
    }

    void store_value(T& value) {
        this->value = &value;
    }

    void store_values(std::vector<T>& values) {
        this->values = &values;
    }

    T get_value() {
        if (this->value != nullptr) {
            return *this->value;
        }

        if (this->default_value != nullptr) {
            return *this->default_value;            
        }
        
        const char* name = this->get_name();
        if (name == nullptr) {
            name = new const char[] {this->get_short_name()};
        }

        std::cerr << "Value not found for argument " << name << '\n';
        exit(EXIT_FAILURE);
    }

    T get_value(size_t index) {
        return (*this->values)[index];
    }

    Argument<T>& mark_multi_value(size_t min_argument_count = 0) {
        this->_is_multi_value = true;
        this->min_argument_count = min_argument_count;
        return *this;
    }

    Argument<T>& mask_positional() {
        this->_is_positional = true;
        this->_is_multi_value = true;
        return *this;
    }
};
}