cmake_minimum_required(VERSION 3.16)
# Set the project name and language
project(Papyrus LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

add_subdirectory(Externals)

add_subdirectory(Nomad)
add_subdirectory(Sand)
add_subdirectory(Artifact)
add_subdirectory(Atum)

add_subdirectory(Merchant)
add_subdirectory(Caravan)
add_subdirectory(Oasis)

add_subdirectory(Samples)
add_subdirectory(Tools)
