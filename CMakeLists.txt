cmake_minimum_required(VERSION 3.20)

project(
    argument_parser
    VERSION 1.0
    DESCRIPTION "C++ Argument Parser"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

option(ARGPARSER_BUILD_TESTS "Build tests" ${MAIN_PROJECT})
option(ARGPARSER_BUILD_EXAMPLE "Build example" ${MAIN_PROJECT})

add_subdirectory(lib)
if (ARGPARSER_BUILD_EXAMPLE) 
    add_subdirectory(bin)
endif()

if (ARGPARSER_BUILD_TESTS) 
    add_subdirectory(tests)
endif()
