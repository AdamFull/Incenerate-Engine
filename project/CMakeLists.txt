cmake_minimum_required(VERSION 3.18.0)
project(engine_project)

if(ANDROID)
add_library(${PROJECT_NAME} main.cpp)
else()
add_executable(${PROJECT_NAME} main.cpp)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE incenerate_engine)
target_include_directories(${PROJECT_NAME} 
	PRIVATE "${MAIN_PROJECT_DIRECTORY_PATH}/engine"
)

#set(BIN_OUTPUT_DIR ${MAIN_PROJECT_DIRECTORY_PATH}/bin/${PROJECT_NAME})
#file(MAKE_DIRECTORY ${BIN_OUTPUT_DIR})
#
#file(GLOB REQ_DYNAMIC_LIBRARIES ${CMAKE_BINARY_DIR}/lib/*.dll ${CMAKE_BINARY_DIR}/lib/*.so)
#
#foreach(lib IN LISTS REQ_DYNAMIC_LIBRARIES)
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                   ${lib}
#                   ${BIN_OUTPUT_DIR})
#endforeach()