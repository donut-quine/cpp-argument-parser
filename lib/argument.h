#pragma once

#include <iostream>
#include <vector>
#include <optional>
#include <charconv>
#include <any>

#include "string_utils.h"

namespace ArgumentParser {

class ArgumentBase {
private:
    char short_name = 0;
    const char* name = nullptr;
    const char* description = nullptr;
public:
    ArgumentBase(const char* name, const char* description = nullptr);

    ArgumentBase(const char short_name, const char* name, const char* description = nullptr);

    virtual ~ArgumentBase() = default;
    
    virtual bool parse_value(const char* string_value) = 0;

    virtual std::any get_value() = 0;

    virtual std::any get_value(size_t index) = 0;

    virtual bool should_have_argument() = 0;

    virtual bool has_default_value() = 0;

    virtual bool has_value() = 0;

    virtual bool is_positional() = 0;

    virtual bool is_multi_value() = 0;

    virtual size_t get_min_value_count() = 0;

    virtual size_t get_value_count() = 0;

    const char* get_name();

    const char get_short_name();

    const char* get_description();
};

template<typename T>
using ParserFunction = std::optional<T> (*)(const char*, std::optional<T> default_value);

template <typename T, ParserFunction<T> parse> class Argument : public ArgumentBase {
private:
    std::optional<T> default_value = std::nullopt;

    T* value = nullptr;
    std::vector<T>* values = nullptr;

    bool owned = true;
    bool _has_value = false;

    bool _should_have_argument = true;
    bool _is_positional = false;

    bool _is_multi_value = false;
    size_t min_argument_count = 0;

    bool set_value(T value) {
        if (this->_has_value) {
            return false;
        }
        
        if (this->value == nullptr) {
            this->value = new T;
        }
        
        *this->value = value;
        this->_has_value = true;
        return true;
    }

    bool add_value(T value) {
        if (this->values == nullptr) {
            this->values = new std::vector<T>();
        }
        
        this->values->push_back(value);
        return true;
    }
public:
    Argument(const char* name, const char* description = nullptr) : ArgumentBase(name, description) {}

    Argument(const char short_name, const char* name, const char* description = nullptr) : ArgumentBase(short_name, name, description) {}

    ~Argument() override {
        if (this->owned) {
            delete this->value;
            delete this->values;
        }
    }
    
    bool parse_value(const char* string_value) override {
        std::optional<T> optional_value = parse(string_value, this->default_value);
        if (!optional_value.has_value()) {
            return false;
        }
        
        if (this->_is_multi_value) {
            return add_value(optional_value.value());
        }

        return set_value(optional_value.value());
    }

    bool should_have_argument() override {
        return this->_should_have_argument;
    }

    bool has_default_value() override {
        return this->default_value.has_value();
    }

    bool has_value() override {
        return this->_has_value;
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
        this->default_value = value;
    }

    void store_value(T& value) {
        this->value = &value;
        this->owned = false;
    }

    void store_values(std::vector<T>& values) {
        this->values = &values;
        this->owned = false;
    }

    std::any get_value() override {
        if (this->value != nullptr) {
            return *this->value;
        }

        if (this->default_value.has_value()) {
            return this->default_value.value();            
        }
        
        return std::any();
    }

    std::any get_value(size_t index) override {
        return (*this->values)[index];
    }

    T get_value_unsafe() {
        std::any value = get_value();
        return std::any_cast<T>(value);
    }

    Argument& mark_multi_value(size_t min_argument_count = 0) {
        this->_is_multi_value = true;
        this->min_argument_count = min_argument_count;
        return *this;
    }

    Argument& mask_positional() {
        this->_is_positional = true;
        this->_is_multi_value = true;
        return *this;
    }
};

template <typename T>
std::optional<T> parse_from_chars(const char* string_value, std::optional<T> default_value) {
    T value;

    const char* begin = string_value;
    const char* end = string_value + strlen(string_value);

    auto [pointer, error_code] = std::from_chars(begin, end, value);

    if (error_code != std::errc()) {
        return std::nullopt;
    }

    // Note: value was not parsed to the end.
    if (pointer != end) {
        return std::nullopt;
    }

    return value;
}

std::optional<std::string> parse_string(const char* string_value, std::optional<std::string> default_value);

std::optional<bool> parse_flag(const char* string_value, std::optional<bool> default_value);

typedef Argument<int, parse_from_chars<int>> IntArgument;
typedef Argument<std::string, parse_string> StringArgument;
typedef Argument<bool, parse_flag> FlagArgument;

}