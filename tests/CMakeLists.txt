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

add_test(react-tests react-tests)
