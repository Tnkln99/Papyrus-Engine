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
target_link_libraries(Merchant PUBLIC flatbuffers)

# --------------------------- Serialization data generation with flatbuffers -------------------------
set(SCHEMES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Schemes)
set(FLATBUFFERS_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Includes/Mrc/Generated)

# Find all .fbs files in the Schemes directory
file(GLOB FBS_FILES ${SCHEMES_DIR}/*.fbs)

# List to hold all generated headers
set(GENERATED_HEADERS)

# Loop over each .fbs file and set up a custom command for flatc
foreach(FBS_FILE ${FBS_FILES})
    # Extract the file name without the extension (e.g., Model from Model.fbs)
    get_filename_component(FBS_BASENAME ${FBS_FILE} NAME_WE)

    # Define the output header for this schema
    set(GENERATED_HEADER ${FLATBUFFERS_OUTPUT_DIR}/${FBS_BASENAME}_generated.h)
    list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})

    # Custom command to run flatc for this schema
    add_custom_command(
            OUTPUT ${GENERATED_HEADER}
            COMMAND flatc
            --cpp -o ${FLATBUFFERS_OUTPUT_DIR} ${FBS_FILE}
            DEPENDS ${FBS_FILE} # Re-run if the schema file changes
            COMMENT "Generating FlatBuffers header for ${FBS_FILE}..."
    )
endforeach()

# Add a custom target to generate all FlatBuffers headers
add_custom_target(GenerateFlatBuffersHeaders ALL
        DEPENDS ${GENERATED_HEADERS}
)


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



