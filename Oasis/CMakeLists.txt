cmake_minimum_required(VERSION 3.16)

# Set the project name and language
project(Oasis LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files in the src directory
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Includes/*.h")

# Add the library target named Oasis
add_library(Oasis STATIC ${SOURCES})

# Include directories for the Oasis library
target_include_directories(Oasis PUBLIC Includes)
target_link_libraries(Oasis PRIVATE Nomad Imgui)

# Use CMake's target_precompile_headers for other compilers
target_precompile_headers(Oasis PUBLIC "Includes/Os/Pch.h")
