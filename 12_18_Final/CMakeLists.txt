# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Battleship VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the source directory
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Collect all source files in the src directory
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)

# Add the executable
add_executable(Battleship ${SOURCES})

# Include the src directory to find header files
target_include_directories(Battleship PRIVATE ${SRC_DIR})
