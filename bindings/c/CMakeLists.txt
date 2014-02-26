set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_library(react_c STATIC react_c.h react_c.hpp react_c.cpp)

if(UNIX OR MINGW)
	set_target_properties(react_c PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

