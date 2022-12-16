cmake_minimum_required(VERSION 3.19.1)
project(incenerateengine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)

set(MAIN_PROJECT_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
        message("MSVC Release build")
        add_compile_options(/Od /Ob0 /Zc:inline /Zp16 /Oi /Ot)
    endif()

	add_compile_options(/W4 /bigobj -D_WIN32_WINNT=0x0601)
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
endif()

add_compile_definitions(
    GLM_FORCE_INLINE 
    GLM_FORCE_XYZW_ONLY
    GLM_CORCE_PURE
    GLM_FORCE_CXX17
    GLM_FORCE_ALIGNED_GENTYPES
    GLM_ENABLE_EXPERIMENTAL
    GLM_EXT_INCLUDED
    MAIN_PROJECT_DIRECTORY_PATH="${MAIN_PROJECT_DIRECTORY_PATH}"
    )

add_subdirectory(engine)

if(ANDROID)
add_library(${PROJECT_NAME} main.cpp)
else()
add_executable(${PROJECT_NAME} main.cpp)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE voxel_engine)
target_include_directories(${PROJECT_NAME} PRIVATE engine)