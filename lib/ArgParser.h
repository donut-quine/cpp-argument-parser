#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace ArgumentParser {

template <typename T>
class ArgumentParser {
public:
    virtual T parse_value(const char* string) const;
};

class ArgumentBase {
private:
    char short_name = 0;
    const char* name = nullptr;
    const char* description = nullptr;
public:
    ArgumentBase(const char* name, const char* description = nullptr);

    ArgumentBase(const char short_name, const char* name, const char* description = nullptr);
    
    virtual void parse_value(const char* string_value);

    virtual bool should_have_argument();

    const char* get_name();

    const char get_short_name();
    
    const char* get_description();
};

template <typename T> class Argument : public ArgumentBase {
private:
    const ArgumentParser<T>* parser = nullptr;

    T default_value;
    T* value = nullptr;
    
    std::vector<T>* values = nullptr;

    bool is_multi_value = false;
    bool is_positional = false;
    bool _should_have_argument = true;

    size_t min_argument_count = 0;
public:
    Argument(const ArgumentParser<T>* parser, const char* name, const char* description = nullptr);

    Argument(const ArgumentParser<T>* parser, const char short_name, const char* name, const char* description = nullptr);
    
    virtual void parse_value(const char* string_value) override;

    virtual bool should_have_argument() override;

    void set_should_have_argument(bool value);

    void Default(T default_value);

    T GetDefault();

    void StoreValue(T& value);
    
    void StoreValues(std::vector<T>& values);

    T GetValue();

    T GetValue(size_t index);

    Argument<T> MultiValue(size_t min_argument_count = 0);

    Argument<T> Positional();
};

class ArgParser {
private:
    const char* name;

    std::vector<ArgumentBase*>* arguments = nullptr;
    std::vector<const char*>* positional_arguments = nullptr;

    bool parse_single_argument(const char* arg, const char* next_arg, bool may_next_argument_be_free);

    ArgumentBase* find_argument_by_name(const char* argument_name);
public:
    ArgParser(const char* name);

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