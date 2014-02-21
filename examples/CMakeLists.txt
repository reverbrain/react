project (react)

cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

message(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "--std=c++0x -Wall")

add_executable(example example.cpp)
