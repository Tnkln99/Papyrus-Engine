cmake_minimum_required(VERSION 3.7)

# Set the project name and language
project(Merchant LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Add the static library target named Merchant
add_library(Merchant STATIC)

# ----------------------------  Add source files to the target  --------------------------------------
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Includes/*.h")
target_sources(Merchant PRIVATE ${SOURCES})

# ----------------------------  Add source includes to the target  -----------------------------------
target_include_directories(Merchant PUBLIC Includes)

# ------------------------------------  add libaries  -------------------------------------------------
target_link_libraries(Merchant PRIVATE Nomad)
target_link_libraries(Merchant PRIVATE assimp)
target_link_libraries(Merchant PRIVATE Artifact)
target_link_libraries(Merchant PRIVATE flatbuffers)

# ----------------------------   add subdirectories  -------------------------------------------------
add_subdirectory(Tests)



