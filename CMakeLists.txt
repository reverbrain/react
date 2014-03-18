cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project(react)

include_directories("foreign/")
include_directories("include/")

add_subdirectory(bindings)
add_subdirectory(examples)
