cmake_minimum_required(VERSION 3.5)

# Define the project name
project(MyProject)

# Define the source files for part1 and part2
set(SOURCES_PART1 part1.c sorting.c random_generator.c utils.c)
set(SOURCES_PART2 part2.c sorting.c random_generator.c utils.c)

# Define the executable name variable
set(EXECUTABLE_NAME "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)

# Check the argument passed to cmake
if("${CMAKE_ARGS}" STREQUAL "part1")
    # Use the part1 source files and set the executable name
    set(SOURCES ${SOURCES_PART1})
    set(EXECUTABLE_NAME "part1")
elseif("${CMAKE_ARGS}" STREQUAL "part2")
    # Use the part2 source files and set the executable name
    set(SOURCES ${SOURCES_PART2})
    set(EXECUTABLE_NAME "part2")
else()
    message(FATAL_ERROR "Invalid argument: ${CMAKE_ARGS}")
endif()

# Set the source files and executable name for the target
add_executable(${EXECUTABLE_NAME} ${SOURCES})

set_target_properties(${EXECUTABLE_NAME}
    PROPERTIES
    BUILD_RPATH ${CMAKE_CURRENT_BINARY_DIR}/..    
)
