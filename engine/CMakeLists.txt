cmake_minimum_required(VERSION 3.18.0)
project(voxel_engine)

set(ENGINE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXTERNALS_PATH "${ENGINE_ROOT}/external")

add_subdirectory("external")

file(GLOB_RECURSE ENGINE_EDITOR_SOURCE "editor/*.cpp" "editor/*.c" "editor/*.cc" "editor/*.h" "editor/*.hpp")
file(GLOB_RECURSE ENGINE_LOADERS_SOURCE "loaders/*.cpp" "loaders/*.c" "loaders/*.cc" "loaders/*.h" "loaders/*.hpp")
file(GLOB_RECURSE ENGINE_PHYSICS_SOURCE "physics/*.cpp" "physics/*.c" "physics/*.cc" "physics/*.h" "physics/*.hpp")
file(GLOB_RECURSE ENGINE_PARTICLES_SOURCE "particles/*.cpp" "particles/*.c" "particles/*.cc" "particles/*.h" "particles/*.hpp")
file(GLOB_RECURSE ENGINE_PARSE_SOURCE "parse/*.cpp" "parse/*.c" "parse/*.cc" "parse/*.h" "parse/*.hpp")
file(GLOB_RECURSE ENGINE_ECS_SOURCE "ecs/*.cpp" "ecs/*.c" "ecs/*.cc" "ecs/*.h" "ecs/*.hpp")
file(GLOB_RECURSE ENGINE_EVENT_SOURCE "event/*.cpp" "event/*.c" "event/*.cc" "event/*.h" "event/*.event")
file(GLOB_RECURSE ENGINE_GRAPHICS_SOURCE "graphics/*.cpp" "graphics/*.c" "graphics/*.cc" "graphics/*.h" "graphics/*.hpp")
file(GLOB_RECURSE ENGINE_AUDIO_SOURCE "audio/*.cpp" "audio/*.c" "audio/*.cc" "audio/*.h" "audio/*.hpp")
file(GLOB_RECURSE ENGINE_SCRIPTING_SOURCE "scripting/*.cpp" "scripting/*.c" "scripting/*.cc" "scripting/*.h" "scripting/*.hpp")
file(GLOB_RECURSE ENGINE_SYSTEM_SOURCE "system/*.cpp" "system/*.c" "system/*.cc" "system/*.h" "system/*.hpp")
file(GLOB_RECURSE ENGINE_GAME_SOURCE "game/*.cpp" "game/*.c" "game/*.cc" "game/*.h" "game/*.hpp")
file(GLOB ENGINE_SOURCE "*.cpp" "*.c" "*.cc" "*.h" "*.hpp")

include_directories(${Vulkan_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC 
${ENGINE_EDITOR_SOURCE}
${ENGINE_LOADERS_SOURCE}
${ENGINE_PARSE_SOURCE}
${ENGINE_PHYSICS_SOURCE}
${ENGINE_PARTICLES_SOURCE}
${ENGINE_ECS_SOURCE}
${ENGINE_EVENT_SOURCE}
${ENGINE_GRAPHICS_SOURCE} 
${ENGINE_AUDIO_SOURCE}
${ENGINE_SCRIPTING_SOURCE}
${ENGINE_SYSTEM_SOURCE}
${ENGINE_GAME_SOURCE}
${ENGINE_SOURCE}
 "ecs/systems/rendering/3d/postprocess/FogEffect.h")

target_link_libraries(${PROJECT_NAME} 
    PUBLIC external_lib
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${Vulkan_INCLUDE_DIR}"
    PUBLIC "${EXTERNALS_PATH}/SDL/include"
    PUBLIC "${EXTERNALS_PATH}/ktx/include"
    PUBLIC "${EXTERNALS_PATH}/openal-soft/include"
    PUBLIC "${EXTERNALS_PATH}/glmlib"
    PUBLIC "${EXTERNALS_PATH}/Effekseer/Dev/Cpp/Effekseer"
    PUBLIC "${EXTERNALS_PATH}/Effekseer/Dev/Cpp/EffekseerRendererVulkan"
    PUBLIC "${ENGINE_ROOT}"
    PUBLIC "${ENGINE_ROOT}/graphics"
    PUBLIC "${ENGINE_ROOT}/system"
    PUBLIC "${ENGINE_ROOT}/game"
)

target_precompile_headers( ${PROJECT_NAME} PUBLIC common.h)