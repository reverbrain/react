set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_library(elliptics_react STATIC elliptics_react.h elliptics_react.hpp elliptics_react.cpp)

if(UNIX OR MINGW)
	set_target_properties(elliptics_react PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

install(TARGETS elliptics_react
	EXPORT EllipticsTargets
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
	BUNDLE DESTINATION library
	)
