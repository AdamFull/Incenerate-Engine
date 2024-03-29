cmake_minimum_required(VERSION 3.18.0)
project(IncenerateFilesystem LANGUAGES CXX)

include(GenerateExportHeader)

option(INCENERATE_FILESYSTEM_BUILD_SHARED "Build incenerate filesystem as a shared library" ON)

file(GLOB VFS_SOURCES_HEAD "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(CPPVFS_SOURCE_FILES ${VFS_SOURCES_HEAD})

file(GLOB VFS_HEADERS_HEAD "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
set(CPPVFS_HEADER_FILES ${VFS_HEADERS_HEAD})

# GPAK FS
file(GLOB VFS_SOURCES_GPAK "${CMAKE_CURRENT_SOURCE_DIR}/gpak/*.cpp")
set(CPPVFS_SOURCE_FILES ${CPPVFS_SOURCE_FILES} ${VFS_SOURCES_GPAK})

file(GLOB VFS_HEADERS_GPAK "${CMAKE_CURRENT_SOURCE_DIR}/gpak/*.h")
set(CPPVFS_HEADER_FILES ${CPPVFS_HEADER_FILES} ${VFS_HEADERS_GPAK})

# Native FS
file(GLOB VFS_SOURCES_NATIVE "${CMAKE_CURRENT_SOURCE_DIR}/native/*.cpp")
set(CPPVFS_SOURCE_FILES ${CPPVFS_SOURCE_FILES} ${VFS_SOURCES_NATIVE})

file(GLOB VFS_HEADERS_NATIVE "${CMAKE_CURRENT_SOURCE_DIR}/native/*.h")
set(CPPVFS_HEADER_FILES ${CPPVFS_HEADER_FILES} ${VFS_HEADERS_NATIVE})

# ZIP FS
file(GLOB VFS_SOURCES_ZIP "${CMAKE_CURRENT_SOURCE_DIR}/zip/*.cpp")
set(CPPVFS_SOURCE_FILES ${CPPVFS_SOURCE_FILES} ${VFS_SOURCES_ZIP})

file(GLOB VFS_HEADERS_ZIP "${CMAKE_CURRENT_SOURCE_DIR}/zip/*.h")
set(CPPVFS_HEADER_FILES ${CPPVFS_HEADER_FILES} ${VFS_HEADERS_ZIP})


if(INCENERATE_FILESYSTEM_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${CPPVFS_SOURCE_FILES} ${CPPVFS_HEADER_FILES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DIncenerateFilesystem_EXPORTS)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${ENGINE_BINARIES_PATH}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${ENGINE_BINARIES_PATH}
    )
else()
    add_library(${PROJECT_NAME} STATIC ${CPPVFS_SOURCE_FILES} ${CPPVFS_HEADER_FILES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DINCENERATEFILESYSTEM_STATIC_DEFINE)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/libzip")
target_link_libraries(${PROJECT_NAME} PRIVATE libzip::zip)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/pak_archive/lib")
target_link_libraries(${PROJECT_NAME} PRIVATE libgpak)

set_target_properties(zip libgpak PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${ENGINE_BINARIES_PATH}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${ENGINE_BINARIES_PATH}
)

generate_export_header(${PROJECT_NAME} 
              EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/ie_vfs_exports.h
              EXPORT_MACRO_NAME INCENERATE_VFS_EXPORT
              NO_EXPORT_MACRO_NAME INCENERATE_VFS_NO_EXPORT
              DEPRECATED_MACRO_NAME INCENERATE_VFS_DEPRECATED
              NO_DEPRECATED_MACRO_NAME INCENERATE_VFS_NO_DEPRECATED
              )

target_include_directories(${PROJECT_NAME}  
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libzip/lib"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/pak_archive/lib"
)