cmake_minimum_required(VERSION 3.18.0)
project(IncenerateAudio)

include(GenerateExportHeader)

option(INCENERATE_AUDIO_BUILD_SHARED "Build incenerate audio as a shared library" ON)

file(GLOB INCENERATE_ENGINE_AUDIO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/loaders/*.cpp")
file(GLOB INCENERATE_ENGINE_AUDIO_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/loaders/*.h")

if(INCENERATE_AUDIO_BUILD_SHARED)
add_library(${PROJECT_NAME} SHARED ${INCENERATE_ENGINE_AUDIO_SOURCES} ${INCENERATE_ENGINE_AUDIO_HEADERS})
target_compile_definitions(${PROJECT_NAME} PRIVATE -DIncenerateAudio_EXPORTS)
else()
add_library(${PROJECT_NAME} STATIC ${INCENERATE_ENGINE_AUDIO_SOURCES} ${INCENERATE_ENGINE_AUDIO_HEADERS})
target_compile_definitions(${PROJECT_NAME} PUBLIC -DINCENERATEAUDIO_STATIC_DEFINE)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_subdirectory(lib/openal-soft)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC OpenAL
    PUBLIC utility_lib
    PRIVATE IncenerateFilesystem
)

generate_export_header(${PROJECT_NAME} 
              EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/ie_audio_exports.h
              EXPORT_MACRO_NAME INCENERATE_AUDIO_EXPORT
              NO_EXPORT_MACRO_NAME INCENERATE_AUDIO_NO_EXPORT
              DEPRECATED_MACRO_NAME INCENERATE_AUDIO_DEPRECATED
              NO_DEPRECATED_MACRO_NAME INCENERATE_AUDIO_NO_DEPRECATED
              )

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/openal-soft/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../external"
)