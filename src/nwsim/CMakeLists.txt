cmake_minimum_required (VERSION 2.6)

# Appends the location of qt5 to CMAKE_PREFIX_PATH **UNIQUE TO HOST**
# list(APPEND CMAKE_PREFIX_PATH "~/usr/lib/")
# list(REMOVE_ITEM CMAKE_PREFIX_PATH "~/usr/lib/")

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

# Find Qt5 Core library components
find_package(Qt5 COMPONENTS Core REQUIRED)

add_executable(${EXECUTABLE_NAME} ${SOURCES})
set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(nwsim-cli Qt5::Core)

