cmake_minimum_required(VERSION 3.19.1)
project(external_lib)

add_library(${PROJECT_NAME} external.cpp)

add_subdirectory(utility)
target_link_libraries(${PROJECT_NAME} PUBLIC utility_lib)

add_subdirectory(mimalloc)
target_link_libraries(${PROJECT_NAME} PUBLIC mimalloc)

add_subdirectory(SDL)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2main SDL2::SDL2)

add_subdirectory(spirvcross)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-core spirv-cross-glsl)

add_subdirectory(glslanglib)
target_link_libraries(${PROJECT_NAME} PUBLIC glslang SPIRV)

add_subdirectory(ktx)
target_link_libraries(${PROJECT_NAME} PUBLIC ktx)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})

target_include_directories(${PROJECT_NAME} 
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)