cmake_minimum_required(VERSION 3.19.1)
project(voxel_engine)

set(ENGINE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXTERNALS_PATH "${ENGINE_ROOT}/external")

add_subdirectory(external)
add_subdirectory(graphics)
add_subdirectory(system)

file(GLOB ENGINE_SOURCE *.cpp *.c *.cc *.h *.hpp)

add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCE})
target_link_libraries(${PROJECT_NAME} 
    PUBLIC external_lib
    PUBLIC engine_graphics
    PUBLIC engine_system
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE "${ENGINE_ROOT}/graphics"
    PRIVATE "${ENGINE_ROOT}/system"
    PUBLIC "${EXTERNALS_PATH}/utility"
    PUBLIC "${EXTERNALS_PATH}/glm"
)