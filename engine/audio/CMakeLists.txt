cmake_minimum_required(VERSION 3.18.0)
project(IncenerateAudio)

file(GLOB INCENERATE_ENGINE_AUDIO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/loaders/*.cpp")
file(GLOB INCENERATE_ENGINE_AUDIO_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/loaders/*.h")

add_library(${PROJECT_NAME} STATIC ${INCENERATE_ENGINE_AUDIO_SOURCES} ${INCENERATE_ENGINE_AUDIO_HEADERS})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_subdirectory(lib/openal-soft)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC OpenAL
    PUBLIC utility_lib
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/openal-soft/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../external"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../external/utility/"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../external/glmlib/"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../shared/"
)