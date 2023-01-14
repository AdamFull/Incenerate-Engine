cmake_minimum_required(VERSION 3.19.1)
project(tfd)

file(GLOB_RECURSE TFD_DIALOG_SOURCES *.c *.h)

add_library(${PROJECT_NAME} STATIC ${TFD_DIALOG_SOURCES})
