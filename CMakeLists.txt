cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project(react)

FILE (READ "${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog" DEBCHANGELOG)

string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" DEBFULLVERSION "${DEBCHANGELOG}")
STRING(REGEX MATCH "([0-9]+\\.[0-9]+)" REACT_VERSION_ABI "${DEBFULLVERSION}")
string(REPLACE "." ";" VERSION_LIST ${DEBFULLVERSION})
list(GET VERSION_LIST 0 REACT_VERSION_0)
list(GET VERSION_LIST 1 REACT_VERSION_1)
list(GET VERSION_LIST 2 REACT_VERSION_2)

add_definitions(-DCONFIG_REACT_VERSION_0=${REACT_VERSION_0})
add_definitions(-DCONFIG_REACT_VERSION_1=${REACT_VERSION_1})
add_definitions(-DCONFIG_REACT_VERSION_2=${REACT_VERSION_2})

option(ENABLE_TESTING "Enable testing" ON)
option(ENABLE_EXAMPLES "Enable examples" ON)
option(ENABLE_BENCHMARKING "Enable benchmarking" OFF)

include_directories("foreign/")
include_directories("include/")

add_definitions(-std=c++0x)

if(ENABLE_TESTING)
	add_subdirectory(tests)
endif()

if(ENABLE_EXAMPLES)
	add_subdirectory(examples)
endif()

# Build react library
file(GLOB_RECURSE REACT_HEADERS
	include/react/*.hpp
	include/react/*.h
)

file(GLOB_RECURSE REACT_SOURCES
	src/*.cpp
)

add_library(react SHARED
	${REACT_HEADERS}
	${REACT_SOURCES}
)

set_target_properties(react PROPERTIES
	VERSION ${DEBFULLVERSION}
	SOVERSION ${REACT_VERSION_ABI}
	COMPILE_FLAGS "-std=c++0x -W -Wall -Werror -pedantic"
	LINKER_LANGUAGE CXX
)

if(UNIX OR MINGW)
	set_target_properties(react PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

install(TARGETS react
	EXPORT ReactTargets
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
	BUNDLE DESTINATION library
)

install(DIRECTORY include/react/
	DESTINATION include/react
)

install(DIRECTORY foreign/
	DESTINATION include/react
)

set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(INSTALL_LIBRARY_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
