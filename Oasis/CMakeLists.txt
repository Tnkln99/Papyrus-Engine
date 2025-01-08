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
target_link_libraries(Oasis PRIVATE Nomad)

# Precompiled headers setup for Oasis
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC compiler setup for precompiled headers
    set(PCH_HEADER "Includes/Os/Pch.h")
    set(PCH_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Pch.h.gch")

    # Compile the precompiled header
    add_custom_command(
            OUTPUT "${PCH_OUTPUT}"
            COMMAND "${CMAKE_CXX_COMPILER}" -x c++-header ${CMAKE_CXX_FLAGS} -o "${PCH_OUTPUT}" "${PCH_HEADER}"
            DEPENDS "${PCH_HEADER}"
    )

    # Ensure the precompiled header is built before the executable
    add_custom_target(Pch DEPENDS "${PCH_OUTPUT}")
    add_dependencies(Oasis Pch)

    # Include the precompiled header in compilation
    target_compile_options(Oasis PUBLIC -include "${PCH_HEADER}")
else()
    # Use CMake's target_precompile_headers for other compilers
    target_precompile_headers(Oasis PUBLIC "Includes/Os/Pch.h")
endif()
