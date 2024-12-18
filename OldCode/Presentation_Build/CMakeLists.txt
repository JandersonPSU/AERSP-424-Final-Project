cmake_minimum_required(VERSION 3.10)

# Project name
project(Battleship)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(Battleship
    src/main.cpp
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Link other sources to the main executable
target_sources(Battleship PRIVATE
    src/game.cpp
    src/player.cpp
)