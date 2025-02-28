cmake_minimum_required(VERSION 3.16)

add_subdirectory(googletest)
# ----------------------------   assimp settings -----------------------------------------------------
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_IMPORT_OBJ ON)

set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
# -----------------------------------------------------------------------------------------------------
add_subdirectory(assimp)
add_subdirectory(DirectXMesh)
add_subdirectory(imgui-docking)
add_subdirectory(flatbuffers)
