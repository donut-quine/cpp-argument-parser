#include "help_formatter.h"

namespace ArgumentParser {

std::string DefaultHelpFormatter::format_argument_description(ArgumentBase& argument) const {
    std::vector<std::string>* description_fragments = new std::vector<std::string>();        

    if (argument.get_short_name() != 0) {
        description_fragments->emplace_back(new char[] {'-', argument.get_short_name(), 0});
    }

    if (argument.get_name() != nullptr) {
        description_fragments->push_back(std::string("--") + argument.get_name());
    }

    if (argument.get_description() != nullptr) {
        description_fragments->emplace_back(argument.get_description());
    }
    
    std::string description = join_strings(*description_fragments, ",\t");
    delete description_fragments;

    return description;
}

std::string DefaultHelpFormatter::format(const char* name, const char* parser_description, std::vector<ArgumentBase*>& arguments) const {
    std::vector<std::string>* description_lines = new std::vector<std::string>();
    description_lines->emplace_back(name);

    if (parser_description != nullptr) {
        description_lines->emplace_back(parser_description);
    }

    size_t positional_argument_count = 0;
    size_t argument_count = 0;
    size_t option_count = 0;
    
    for (size_t i = 0; i < arguments.size(); i++) {
        ArgumentBase* argument = arguments[i];

        if (argument->is_positional()) {
            positional_argument_count++;
            continue;
        }

        if (argument->has_default_value()) {
            option_count++;
            continue;
        }

        argument_count++;
    }

    if (positional_argument_count > 0) {
        description_lines->emplace_back("\nPositional arguments:");

        for (size_t i = 0; i < arguments.size(); i++) {
            ArgumentBase* argument = arguments[i];

            if (!argument->is_positional()) {
                continue;
            }
            
            description_lines->push_back(this->format_argument_description(*argument));
        }
    }

    if (argument_count > 0) {
        description_lines->emplace_back("\nArguments:");

        for (size_t i = 0; i < arguments.size(); i++) {
            ArgumentBase* argument = arguments[i];

            if (argument->is_positional() || argument->has_default_value()) {
                continue;
            }

            description_lines->push_back(this->format_argument_description(*argument));
        }
    }

    if (option_count > 0) {
        description_lines->emplace_back("\nOptions:");

        for (size_t i = 0; i < arguments.size(); i++) {
            ArgumentBase* argument = arguments[i];

            if (argument->is_positional()) {
                continue;
            }

            if (!argument->has_default_value()) {
                continue;
            }
            
            description_lines->push_back(this->format_argument_description(*argument));
        }
    }
        
    std::string description = join_strings(*description_lines, "\n");
    delete description_lines;

    return description;
}

} // namespace ArgumentParser
