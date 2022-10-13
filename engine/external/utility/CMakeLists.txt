cmake_minimum_required(VERSION 3.18)
project(utility_lib)

file(GLOB UTILITY_MEMORY_SOURCES *.cpp *.hpp *.h)

add_library(${PROJECT_NAME} ${UTILITY_MEMORY_SOURCES})