cmake_minimum_required(VERSION 3.16)
# Set the project name and language
project(Papyrus LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Externals)

add_subdirectory(Nomad)
add_subdirectory(Sand)
add_subdirectory(Merchant)
add_subdirectory(Caravan)
add_subdirectory(Oasis)

add_executable(Papyrus "Papyrus.cpp")

# Linking against Sand
target_link_libraries(Papyrus PRIVATE Nomad Caravan Oasis)
