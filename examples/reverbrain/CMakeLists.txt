set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_library(reverbrain_react STATIC reverbrain_react.h reverbrain_react.hpp reverbrain_react.cpp)

if(UNIX OR MINGW)
	set_target_properties(reverbrain_react PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

install(TARGETS reverbrain_react
	EXPORT EllipticsTargets
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
	BUNDLE DESTINATION library
	)
