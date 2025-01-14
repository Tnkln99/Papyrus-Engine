cmake_minimum_required(VERSION 3.16)

# Set the project name and language
project(CaravanTests LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CaravanTests)

# Collect all source files in the src directory
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Includes/*.h")

# Add sources to the CaravanTests lib
target_sources(CaravanTests PRIVATE ${SOURCES})

target_link_libraries(CaravanTests PRIVATE Sand)
target_link_libraries(CaravanTests PRIVATE Nomad)
target_link_libraries(CaravanTests PRIVATE Caravan)
target_link_libraries(CaravanTests PRIVATE gtest_main)

# Include directories for the CaravanTests libary
target_include_directories(CaravanTests PUBLIC Includes)

# Platform-specific settings
if(WIN32)
    # Link DirectX 12 libraries if needed by CaravanTests
    target_link_libraries(CaravanTests PRIVATE
            d3d12.lib
            dxgi.lib
            dxguid.lib
    )
endif()
