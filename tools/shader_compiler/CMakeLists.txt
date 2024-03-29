cmake_minimum_required(VERSION 3.18.0)
project(shader_compiler)

file(GLOB_RECURSE COMPILER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE COMPILER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(${PROJECT_NAME} ${COMPILER_SOURCES} ${COMPILER_HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TOOLS_BINARIES_PATH}/${PROJECT_NAME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TOOLS_BINARIES_PATH}/${PROJECT_NAME}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TOOLS_BINARIES_PATH}/${PROJECT_NAME}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${TOOLS_BINARIES_PATH}/${PROJECT_NAME}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${TOOLS_BINARIES_PATH}/${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC external_lib
)
target_include_directories(${PROJECT_NAME} 
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
	PRIVATE "${MAIN_PROJECT_DIRECTORY_PATH}/engine/external/glslanglib"
	PRIVATE "${MAIN_PROJECT_DIRECTORY_PATH}/engine/external/utility"
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
