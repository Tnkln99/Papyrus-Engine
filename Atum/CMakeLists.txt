cmake_minimum_required(VERSION 3.7)

# Set the project name and language
project(Atum LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Add the static library target named Atum
add_library(Atum STATIC)

# ----------------------------  Add source files to the target  --------------------------------------
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Includes/*.h")
target_sources(Atum PRIVATE ${SOURCES})

# ----------------------------  Add source includes to the target  -----------------------------------
target_include_directories(Atum PUBLIC Includes)

# ------------------------------------  add libaries  -------------------------------------------------
target_link_libraries(Atum PRIVATE Nomad)
target_link_libraries(Atum PRIVATE Artifact)
target_link_libraries(Atum PRIVATE DirectXMesh)


