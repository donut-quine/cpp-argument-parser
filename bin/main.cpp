#include <functional>
#include <argparser.h>

#include <iostream>
#include <numeric>

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<int> values;

    ArgumentParser::ArgParser parser("Program");
    parser.add_int_argument("N").mark_multi_value(1).mask_positional().store_values(values);
    parser.add_flag("sum", "add args").store_value(opt.sum);
    parser.add_flag("mult", "multiply args").store_value(opt.mult);
    parser.add_help('h', "help", "Program accumulate arguments");

    if(!parser.parse(argc, argv)) {
        std::cout << "Wrong argument" << std::endl;
        std::cout << parser.get_help_description() << std::endl;
        return 1;
    }

    if(parser.help()) {
        std::cout << parser.get_help_description() << std::endl;
        return 0;
    }

    if(opt.sum) {
        std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 0) << std::endl;
    } else if(opt.mult) {
        std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<int>()) << std::endl;
    } else {
        std::cout << "No one options had chosen" << std::endl;
        std::cout << parser.get_help_description();
        return 1;
    }

    return 0;

}
