set(TARGET argparser)

add_library(${TARGET})

# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)

# add *.h and *.cpp files
# Note: According to cpr author's statement, this is useful for IDEs
target_sources(${TARGET} PRIVATE ${HEADERS})
target_sources(${TARGET} PRIVATE ${SOURCES})

target_include_directories(${TARGET} PUBLIC source)

# set_target_properties(${TARGET} PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
# )
