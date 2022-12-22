cmake_minimum_required(VERSION 3.18.0)
project(VulkanMemoryAllocator)

file(GLOB VMA_SOURCES *.cpp *.c *.cc)

add_library(${PROJECT_NAME} STATIC ${VMA_SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${Vulkan_INCLUDE_DIR}"
)