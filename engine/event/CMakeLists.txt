cmake_minimum_required(VERSION 3.18.0)
project(IncenerateEvents)

include(GenerateExportHeader)

option(INCENERATE_EVENT_BUILD_SHARED "Build incenerate event system as a shared library" ON)

file(GLOB INCENERATE_ENGINE_EVENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB INCENERATE_ENGINE_EVENT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

if(INCENERATE_EVENT_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${INCENERATE_ENGINE_EVENT_SOURCES} ${INCENERATE_ENGINE_EVENT_HEADERS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DIncenerateEvents_EXPORTS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${ENGINE_BINARIES_PATH}
    )
else()
    add_library(${PROJECT_NAME} STATIC ${INCENERATE_ENGINE_EVENT_SOURCES} ${INCENERATE_ENGINE_EVENT_HEADERS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DINCENERATEEVENTS_STATIC_DEFINE)
endif()


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

generate_export_header(${PROJECT_NAME} 
              EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/ie_event_exports.h
              EXPORT_MACRO_NAME INCENERATE_EVENT_EXPORT
              NO_EXPORT_MACRO_NAME INCENERATE_EVENT_NO_EXPORT
              DEPRECATED_MACRO_NAME INCENERATE_EVENT_DEPRECATED
              NO_DEPRECATED_MACRO_NAME INCENERATE_EVENT_NO_DEPRECATED
              )

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)