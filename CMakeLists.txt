cmake_minimum_required(VERSION 3.12)

project(
    argument_parser
    VERSION 1.0
    DESCRIPTION "C++ Argument Parser"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(tests)
