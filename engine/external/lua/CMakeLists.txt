cmake_minimum_required(VERSION 3.19.1)
project(lualib)

file(GLOB_RECURSE LUA_SOURCES *.cpp *.c *.cc)

add_library(${PROJECT_NAME} STATIC ${LUA_SOURCES})