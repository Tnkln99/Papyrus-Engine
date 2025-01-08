cmake_minimum_required(VERSION 3.16)

# Set the project name and language
project(MerchantTests LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MerchantTests)

# Collect all source files in the scr directory
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Src/*.h")

# Add sources
target_sources(MerchantTests PRIVATE ${SOURCES})

# Link
target_link_libraries(MerchantTests PRIVATE Alchemist)
target_link_libraries(MerchantTests PUBLIC gtest_main)