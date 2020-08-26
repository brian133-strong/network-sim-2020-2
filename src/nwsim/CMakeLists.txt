cmake_minimum_required (VERSION 2.6)
project (networksim)
#include_directories("src")

# Set the name of the executable to be produced
set(EXECUTABLE_NAME nwsim-cli)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} /../../bin/)

# Use C++17
SET(CMAKE_CXX_STANDARD 17)

# Add all .cpp - files under src/ directory
#file(GLOB SOURCES src/*.cpp)
file(GLOB SOURCES *.cpp)
add_executable(${EXECUTABLE_NAME} ${SOURCES})
set(CMAKE_BUILD_TYPE Debug)