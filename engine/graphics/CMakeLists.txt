cmake_minimum_required(VERSION 3.19.1)
project(engine_graphics)

file(GLOB_RECURSE ENGINE_GRAPHICS_SOURCE *.cpp *.c *.cc *.h *.hpp)

add_library(${PROJECT_NAME} STATIC ${ENGINE_GRAPHICS_SOURCE})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${ENGINE_ROOT}"
    PUBLIC "${EXTERNALS_PATH}"
    PUBLIC "${EXTERNALS_PATH}/glm"
    PUBLIC "${EXTERNALS_PATH}/SDL/include"
    PUBLIC "${Vulkan_INCLUDE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)