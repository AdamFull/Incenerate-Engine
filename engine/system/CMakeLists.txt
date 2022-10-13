cmake_minimum_required(VERSION 3.19.1)
project(engine_system)

file(GLOB_RECURSE ENGINE_SYSTEM_SOURCES *.cpp *.c *.cc *.h *.hpp)

add_library(${PROJECT_NAME} STATIC ${ENGINE_SYSTEM_SOURCES})

target_include_directories(${PROJECT_NAME} 
	PRIVATE "${EXTERNALS_PATH}/SDL/include"
	PUBLIC "${EXTERNALS_PATH}"
	PRIVATE "${ENGINE_ROOT}/system"
	PUBLIC "${ENGINE_ROOT}"
)