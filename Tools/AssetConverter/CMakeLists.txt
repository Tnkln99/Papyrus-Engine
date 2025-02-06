cmake_minimum_required(VERSION 3.16)
# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AssetConverter)

# Collect all source files in the scr directory
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Src/*.h")

# Add sources
target_sources(AssetConverter PRIVATE ${SOURCES})
target_include_directories(AssetConverter PRIVATE ${SOURCES})
target_include_directories(AssetConverter PUBLIC Src)


# Linking against Sand
target_link_libraries(AssetConverter PRIVATE Nomad Oasis Caravan Imgui Merchant)