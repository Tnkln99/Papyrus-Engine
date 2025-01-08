cmake_minimum_required(VERSION 3.16)

# Set the project name and language
project(SandTests LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SandTests)

# Collect all source files in the scr directory
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Src/*.h")

# Add sources
target_sources(SandTests PRIVATE ${SOURCES})

# Link
target_link_libraries(SandTests PRIVATE Sand)
target_link_libraries(SandTests PUBLIC gtest_main)

target_link_libraries(SandTests PRIVATE
        d3d12.lib
        dxgi.lib
        dxguid.lib
)

target_precompile_headers(SandTests PRIVATE "Src/Pch.h")
