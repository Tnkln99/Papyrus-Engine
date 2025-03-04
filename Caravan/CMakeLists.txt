cmake_minimum_required(VERSION 3.16)

# Set the project name and language
project(Caravan LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable target named Caravan
add_library(Caravan STATIC)

# Collect all source files in the src directory
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Includes/*.h")

# Add sources to the Caravan lib
target_sources(Caravan PRIVATE ${SOURCES})

target_link_libraries(Caravan PRIVATE Sand)
target_link_libraries(Caravan PRIVATE Nomad)

# Include directories for the Caravan libary
target_include_directories(Caravan PUBLIC Includes)

include_directories(Shaders)

# Find and compile all shaders
file(GLOB_RECURSE SHADERS "Shaders/*.hlsl")
set(GENERATED_HEADERS "")

foreach(SHADER_FILE ${SHADERS})
    # Get the directory of the shader file
    get_filename_component(SHADER_DIR ${SHADER_FILE} DIRECTORY)
    # Get the filename without extension
    get_filename_component(FILENAME_WE ${SHADER_FILE} NAME_WE)

    # Define the output path for the generated header (next to the shader file)
    set(GENERATED_HEADER "${SHADER_DIR}/${FILENAME_WE}.h")
    list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})

    # Determine shader type based on the filename prefix (adjust as needed)
    if(FILENAME_WE MATCHES "^Vx")
        set(SHADER_PROFILE "vs_5_0")  # Vertex Shader
    elseif(FILENAME_WE MATCHES "^Px")
        set(SHADER_PROFILE "ps_5_0")  # Pixel Shader
    else()
        message(FATAL_ERROR "Unknown shader type for ${SHADER_FILE}")
    endif()

    # todo: add option D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION if on debug
    # Add custom command to compile shader
    add_custom_command(
            OUTPUT ${GENERATED_HEADER}
            COMMAND dxc -T ${SHADER_PROFILE} -E main -Fh ${GENERATED_HEADER} ${SHADER_FILE} -Vn ${FILENAME_WE} /Zi
            COMMENT "Compiling shader ${SHADER_FILE} to ${GENERATED_HEADER}"
            DEPENDS ${SHADER_FILE}
    )
endforeach()

# Add custom target for shader compilation and header generation
add_custom_target(GenerateShaderHeaders DEPENDS ${GENERATED_HEADERS})
add_dependencies(Caravan GenerateShaderHeaders)


target_precompile_headers(Caravan PUBLIC "Includes/Crv/Pch.h")

target_link_libraries(Caravan PRIVATE
        d3d12.lib
        dxgi.lib
        dxguid.lib
        Imgui
)

add_subdirectory(Tests)
