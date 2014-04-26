add_definitions(-std=c++0x -W -Wall -Werror -pedantic)
add_definitions(-O2)

file(GLOB_RECURSE BENCHMARKS
	benchmark_*.cpp
)

add_executable(react-benchmarks
	benchmarks.hpp
	benchmarks.cpp
	${BENCHMARKS}
)

target_link_libraries(react-benchmarks
	react
	celero
)

add_executable(react-benchmarks-for
	for.cpp
)

target_link_libraries(react-benchmarks-for
	react
)

add_executable(react-benchmarks-recurse
	recurse.cpp
)

target_link_libraries(react-benchmarks-recurse
	react
)
