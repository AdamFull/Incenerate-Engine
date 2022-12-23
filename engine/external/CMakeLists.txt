cmake_minimum_required(VERSION 3.18.0)
project(external_lib)

set(EXTERNAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} external.cpp)

add_subdirectory(utility)
target_link_libraries(${PROJECT_NAME} PUBLIC utility_lib)

add_subdirectory(mimalloc)
target_link_libraries(${PROJECT_NAME} PUBLIC mimalloc)

add_subdirectory(SDL)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)

add_subdirectory(spirvcross)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-core spirv-cross-glsl)

add_subdirectory(glslanglib)
target_link_libraries(${PROJECT_NAME} PUBLIC glslang SPIRV)

add_subdirectory(ktx)
target_link_libraries(${PROJECT_NAME} PUBLIC ktx)

if(ANDROID)
    add_definitions(-DVULKAN_HPP_TYPESAFE_CONVERSION -DVMA_IMPLEMENTATION -DVMA_STATS_STRING_ENABLED=0)
    set(VULKAN_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src)
    set(Vulkan_INCLUDE_DIR ${VULKAN_DIR}/include)
else()
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
endif()

add_subdirectory(vma)
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanMemoryAllocator)

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${Vulkan_INCLUDE_DIR}"
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)