#pragma once

#include "argument.h"

#include <string>
#include <vector>

namespace ArgumentParser {

class AbstractHelpFormatter {
public:
    virtual std::string format(const char* parser_name, const char* parser_description, std::vector<ArgumentBase*>& arguments) const = 0;
};

class DefaultHelpFormatter : public AbstractHelpFormatter {
private:
    std::string format_argument_description(ArgumentBase& argument) const;
public:
    std::string format(const char* parser_name, const char* parser_description, std::vector<ArgumentBase*>& arguments) const override;
};

}