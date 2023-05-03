cmake_minimum_required(VERSION 3.18.0)
project(incenerate-editor)

file(GLOB_RECURSE EDITOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE EDITOR_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(${PROJECT_NAME} ${EDITOR_SOURCES} ${EDITOR_HEADERS})

target_link_libraries(${PROJECT_NAME} 
	PUBLIC voxel_engine
	SDL2::SDL2 SDL2::SDL2main
)
target_include_directories(${PROJECT_NAME} 
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
	PRIVATE "${MAIN_PROJECT_DIRECTORY_PATH}/engine"
	PRIVATE "${MAIN_PROJECT_DIRECTORY_PATH}/engine/external/SDL/include"
	PRIVATE "${MAIN_PROJECT_DIRECTORY_PATH}/engine/external/vulkan/include"
)