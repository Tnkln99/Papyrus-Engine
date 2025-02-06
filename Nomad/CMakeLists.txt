cmake_minimum_required(VERSION 3.16)

# Set the project name and language
project(Nomad LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files in the src directory
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Includes/*.h")

# Add the library target named Nomad
add_library(Nomad STATIC ${SOURCES})

# Include directories for the Nomad library
target_include_directories(Nomad PUBLIC Includes)

target_precompile_headers(Nomad PUBLIC "Includes/Nmd/Pch.h")