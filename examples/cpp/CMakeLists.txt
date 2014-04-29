cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "--std=c++0x -Wall -lpthread")

add_executable(low_level low_level.cpp)

add_executable(high_level high_level.cpp)
target_link_libraries(high_level react)

add_executable(stats stats.cpp)
target_link_libraries(stats react)
