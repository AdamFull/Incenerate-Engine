cmake_minimum_required(VERSION 3.18.0)
project(IncenerateScripting LANGUAGES CXX)

file(GLOB SCRIPTING_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB SCRIPTING_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(${PROJECT_NAME} STATIC ${SCRIPTING_SOURCE_FILES} ${SCRIPTING_HEADER_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/lua")
target_link_libraries(${PROJECT_NAME} PUBLIC lualib)

target_include_directories(${PROJECT_NAME}  
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../external/glmlib/"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../external/utility/"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

target_precompile_headers( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/scripting_pch.h")