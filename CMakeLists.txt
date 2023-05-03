cmake_minimum_required(VERSION 3.18.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(MAIN_PROJECT_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#sdl
set(SDL_TEST OFF CACHE BOOL "Build the SDL2_test library" FORCE)
set(SDL_3DNOW OFF CACHE BOOL "Use 3Dnow! MMX assembly routines" FORCE)
set(SDL_DISKAUDIO OFF CACHE BOOL "Support the disk writer audio driver" FORCE)
set(SDL_DUMMYAUDIO OFF CACHE BOOL "Support the dummy audio driver" FORCE)
set(SDL_DIRECTFB OFF CACHE BOOL "Use DirectFB video driver" FORCE)
set(SDL_DUMMYVIDEO OFF CACHE BOOL "Use dummy video driver" FORCE)
set(SDL_OPENGL OFF CACHE BOOL "Include OpenGL support" FORCE)
set(SDL_OPENGLES OFF CACHE BOOL "Include OpenGL ES support" FORCE)
set(SDL_ALSA OFF CACHE BOOL "Support the ALSA audio API" FORCE)
set(SDL_JACK OFF CACHE BOOL "Support the JACK audio API" FORCE)
set(SDL_ESD OFF CACHE BOOL "Support the Enlightened Sound Daemon" FORCE)
set(SDL_PIPEWIRE OFF CACHE BOOL "Use Pipewire audio" FORCE)
set(SDL_PULSEAUDIO OFF CACHE BOOL "Use PulseAudio" FORCE)
set(SDL_ARTS OFF CACHE BOOL "Support the Analog Real Time Synthesizer" FORCE)
set(SDL_NAS OFF CACHE BOOL "Support the NAS audio API" FORCE)
set(SDL_SNDIO OFF CACHE BOOL "Support the sndio audio API" FORCE)
set(SDL_FUSIONSOUND OFF CACHE BOOL "Use FusionSound audio driver" FORCE)
set(SDL_LIBSAMPLERATE OFF CACHE BOOL "Use libsamplerate for audio rate conversion" FORCE)
set(SDL_X11 OFF CACHE BOOL "Use X11 video driver" FORCE)
set(SDL_WAYLAND OFF CACHE BOOL "Use Wayland video driver" FORCE)
set(SDL_RPI OFF CACHE BOOL "Use Raspberry Pi video driver" FORCE)
set(SDL_COCOA OFF CACHE BOOL "Use Cocoa video driver" FORCE)
set(SDL_DIRECTX OFF CACHE BOOL "Use DirectX for Windows audio/video" FORCE)
set(SDL_XINPUT ON CACHE BOOL "Use Xinput for Windows" FORCE)
set(SDL_WASAPI OFF CACHE BOOL "Use the Windows WASAPI audio driver" FORCE)
set(SDL_RENDER_D3D OFF CACHE BOOL "Enable the Direct3D render driver" FORCE)
set(SDL_RENDER_METAL OFF CACHE BOOL "Enable the Metal render driver" FORCE)
set(SDL_VIVANTE OFF CACHE BOOL "Use Vivante EGL video driver" FORCE)
set(SDL_METAL OFF CACHE BOOL "Enable Metal support" FORCE)
set(SDL_KMSDRM OFF CACHE BOOL "Use KMS DRM video driver" FORCE)
set(SDL_OFFSCREEN OFF CACHE BOOL "Use offscreen video driver" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "Enable the HIDAPI subsystem" FORCE)

#openal-soft
set(ALSOFT_UTILS OFF CACHE BOOL "Build and install utility programs" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build and install example programs" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "Build and install test programs" FORCE)

#libzip
set(BUILD_TOOLS OFF CACHE BOOL "Build tools in the src directory (zipcmp, zipmerge, ziptool)" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "Build regression tests" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_DOC OFF CACHE BOOL "Build documentation" FORCE)

#glslang
set(BUILD_EXTERNAL OFF CACHE BOOL "Build external dependencies in /External" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap" FORCE)
set(ENABLE_CTEST OFF CACHE BOOL "Enables testing" FORCE)

#bullet
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Set when you want to build the Bullet 2 demos" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "Set when you want to build the OpenGL3+ demos" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "Set when you want to build the extras" FORCE)
#set(BUILD_SHARED_LIBS ON CACHE BOOL "Use shared libraries" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Build Unit Tests"	FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "Build original Bullet CPU examples" FORCE)
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "Use Graphical Benchmark" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Use MSVC Runtime Library DLL (/MD or /MDd)" FORCE)

#lz4
set(LZ4_BUILD_CLI OFF CACHE BOOL "Build lz4 program" FORCE)
set(LZ4_BUILD_LEGACY_LZ4C OFF CACHE BOOL "Build lz4c program with legacy argument support" FORCE)

#zstd
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "BUILD PROGRAMS" FORCE)
set(ZSTD_BUILD_TESTS OFF CACHE BOOL "BUILD TESTS" FORCE)

#mimalloc
set(MI_BUILD_STATIC OFF CACHE BOOL "Build static library" FORCE)
set(MI_BUILD_OBJECT OFF CACHE BOOL "Build object library" FORCE)
set(MI_BUILD_TESTS OFF CACHE BOOL "Build test executables" FORCE)

#ktx
set(KTX_FEATURE_TESTS OFF CACHE BOOL "Create unit tests" FORCE)
set(KTX_FEATURE_TOOLS OFF CACHE BOOL "Create KTX tools" FORCE)
#set(KTX_FEATURE_VULKAN OFF CACHE BOOL "Enable Vulkan texture upload" FORCE)
set(KTX_FEATURE_GL_UPLOAD OFF CACHE BOOL "Enable OpenGL texture upload" FORCE)

#spirv-cross
#set(SPIRV_CROSS_SHARED OFF CACHE BOOL "Build the C API as a single shared library." FORCE)
set(SPIRV_CROSS_STATIC ON CACHE BOOL "Build the C and C++ API as static libraries." FORCE)
set(SPIRV_CROSS_CLI OFF CACHE BOOL "Build the CLI binary. Requires SPIRV_CROSS_STATIC." FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "Enable SPIRV-Cross tests." FORCE)
set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "Enable util module support." FORCE)

#backward
set(BACKWARD_SHARED OFF CACHE BOOL "Build dynamic backward-cpp shared lib" FORCE)

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD")

    if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
        message("MSVC Release build")
        add_compile_options(/Od /Ob0 /Zc:inline /Zp16 /Oi /Ot)
    endif()

	add_compile_options(/W4 /bigobj -D_WIN32_WINNT=0x0601 /arch:AVX2)
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
endif()

add_compile_definitions(
    GLM_FORCE_INLINE 
    GLM_FORCE_XYZW_ONLY
    GLM_FORCE_CXX17
    GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
    GLM_FORCE_SIMD_AVX2
    GLM_ENABLE_EXPERIMENTAL
    GLM_EXT_INCLUDED
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VK_NO_PROTOTYPES
    MAIN_PROJECT_DIRECTORY_PATH="${MAIN_PROJECT_DIRECTORY_PATH}"
    )

add_subdirectory(engine)
add_subdirectory(project)
add_subdirectory(editor)

#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/embed/font" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/embed")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/embed/icon" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/embed")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/embed/shaders" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/embed")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/embed/config.json" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/embed")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/embed/materialeditor.json" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/embed")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/embed/shaders.json" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/embed")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/embed/imgui.ini" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")