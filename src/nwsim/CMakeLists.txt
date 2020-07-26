cmake_minimum_required (VERSION 2.6)
project (nwsim)
#include_directories("src")

# Set the name of the executable to be produced
set(EXECUTABLE_NAME nwsim)

# Add all .cpp - files under src/ directory
#file(GLOB SOURCES src/*.cpp)
file(GLOB SOURCES *.cpp)
add_executable(${EXECUTABLE_NAME} ${SOURCES})
set(CMAKE_BUILD_TYPE Debug)