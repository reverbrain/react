add_definitions(-DBOOST_TEST_DYN_LINK)
add_definitions(-std=c++0x -W -Wall -Werror -pedantic)

file(GLOB_RECURSE TESTS
	test_*.cpp
)

add_executable(react-tests
	tests.hpp
	tests.cpp
	${TESTS}
)

target_link_libraries(react-tests
	boost_unit_test_framework
	react
)

set(TEST_LINK_FLAGS "-Wl,-rpath,${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_CURRENT_BINARY_DIR}/../:")

set_target_properties(react-tests PROPERTIES
	LINK_FLAGS "${TEST_LINK_FLAGS}"
	LINKER_LANGUAGE CXX
)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
add_test(react-tests react-tests --log_level=test_suite)
