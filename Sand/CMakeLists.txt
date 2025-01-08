cmake_minimum_required(VERSION 3.7)

# Set the project name and language
project(Sand LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Add the static library target named Sand
add_library(Sand STATIC)

# ----------------------------  Add source files to the target  --------------------------------------
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Includes/*.h")
target_sources(Sand PRIVATE ${SOURCES})

# ----------------------------  Add source includes to the target  -----------------------------------
target_include_directories(Sand PUBLIC Includes)

# ------------------------------------  add libaries  -------------------------------------------------
target_link_libraries(Sand PRIVATE Nomad)

# Link DirectX 12 libraries
target_link_libraries(Sand PRIVATE
        d3d12.lib
        dxgi.lib
        dxguid.lib
)

# ----------------------------   Precompiled headers setup  ------------------------------------------
target_precompile_headers(Sand PUBLIC "Includes/Snd/Pch.h")

# ----------------------------   add subdirectories  -------------------------------------------------
add_subdirectory(Tests)
