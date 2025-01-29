cmake_minimum_required(VERSION 3.7)

# Set the project name and language
project(Imgui LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Add the static library target named Imgui
add_library(Imgui STATIC)

# ----------------------------  Add source files to the target  --------------------------------------
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
target_sources(Imgui PRIVATE ${SOURCES})

# ----------------------------  Add source includes to the target  -----------------------------------
target_include_directories(Imgui PUBLIC "")