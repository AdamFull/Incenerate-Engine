cmake_minimum_required(VERSION 3.19.1)
project(external_lib)

add_library(${PROJECT_NAME} external.cpp)

add_subdirectory(utility)
target_link_libraries(${PROJECT_NAME} utility_lib)

add_subdirectory(mimalloc)
target_link_libraries(${PROJECT_NAME} mimalloc)

add_subdirectory(SDL)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2)

add_subdirectory(glad)
target_link_libraries(${PROJECT_NAME} glad)

add_subdirectory(spirvcross)
target_link_libraries(${PROJECT_NAME} spirv-cross-core spirv-cross-glsl)

add_subdirectory(glslanglib)
target_link_libraries(${PROJECT_NAME} glslang SPIRV)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})

target_include_directories(${PROJECT_NAME} 
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)