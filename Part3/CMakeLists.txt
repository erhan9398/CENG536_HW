cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Set the C standard to C11
set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)

# Add the executable
add_executable(part3 main.c)

# Link pthread lib
target_link_libraries(part3 PRIVATE pthread)