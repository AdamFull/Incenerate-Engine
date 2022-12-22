cmake_minimum_required(VERSION 3.18.0)
project(voxel_engine)

set(ENGINE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXTERNALS_PATH "${ENGINE_ROOT}/external")

add_subdirectory("external")

file(GLOB_RECURSE ENGINE_GRAPHICS_SOURCE "graphics/*.cpp" "graphics/*.c" "graphics/*.cc")
file(GLOB_RECURSE ENGINE_SYSTEM_SOURCE "system/*.cpp" "system/*.c" "system/*.cc")
file(GLOB_RECURSE ENGINE_GAME_SOURCE "game/*.cpp" "game/*.c" "game/*.cc")
file(GLOB ENGINE_SOURCE "*.cpp" "*.c" "*.cc")

include_directories(${Vulkan_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC 
${ENGINE_SOURCE} 
${ENGINE_GRAPHICS_SOURCE}
${ENGINE_SYSTEM_SOURCE}
${ENGINE_GAME_SOURCE})

target_link_libraries(${PROJECT_NAME} 
    PUBLIC external_lib
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${Vulkan_INCLUDE_DIR}"
    PUBLIC "${EXTERNALS_PATH}/SDL/include"
    PUBLIC "${EXTERNALS_PATH}/ktx/include"
    PUBLIC "${ENGINE_ROOT}"
    PUBLIC "${ENGINE_ROOT}/graphics"
    PUBLIC "${ENGINE_ROOT}/system"
    PUBLIC "${ENGINE_ROOT}/game"
)

target_precompile_headers( ${PROJECT_NAME} PUBLIC common.h)