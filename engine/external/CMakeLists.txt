cmake_minimum_required(VERSION 3.18.0)
project(external_lib)

set(EXTERNAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} external.cpp "stb_vorbis.h")

add_subdirectory(utility)
target_link_libraries(${PROJECT_NAME} PUBLIC utility_lib)

add_subdirectory(mimalloc)
target_link_libraries(${PROJECT_NAME} PUBLIC mimalloc)

add_subdirectory(SDL)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2 SDL2::SDL2main)

add_subdirectory(spirvcross)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-core spirv-cross-glsl)

add_subdirectory(glslanglib)
target_link_libraries(${PROJECT_NAME} PUBLIC glslang SPIRV)

add_subdirectory(ktx)
target_link_libraries(${PROJECT_NAME} PUBLIC ktx)

add_subdirectory(imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)

add_subdirectory(tinyfiledialogs)
target_link_libraries(${PROJECT_NAME} PUBLIC tfd)

set(Vulkan_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/include)

add_subdirectory(vulkan)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan-Headers)

add_subdirectory(vma)
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanMemoryAllocator)

add_subdirectory(bullet3)
target_link_libraries(${PROJECT_NAME} PUBLIC BulletCollision)
target_link_libraries(${PROJECT_NAME} PUBLIC BulletDynamics)
target_link_libraries(${PROJECT_NAME} PUBLIC LinearMath)

add_subdirectory(meshoptimizer)
target_link_libraries(${PROJECT_NAME} PUBLIC meshoptimizer)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "vulkan/include"
    PUBLIC "SDL/include"
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(mimalloc SDL2 SDL2main ktx ktx_read BulletCollision BulletDynamics LinearMath PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${ENGINE_BINARIES_PATH}
)