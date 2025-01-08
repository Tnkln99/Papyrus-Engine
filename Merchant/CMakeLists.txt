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

# ----------------------------   assimp settings -----------------------------------------------------
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_IMPORT_OBJ ON)

set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)

# ----------------------------   add subdirectories  -------------------------------------------------
add_subdirectory(Externals)
add_subdirectory(Tests)



