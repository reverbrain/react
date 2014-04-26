#include <iostream>

#include "react/react.h"

size_t ITERATIONS_NUMBER = 1000000;

int main() {

	size_t x = 2;
	size_t start_time = clock();

	int action_code = react_define_new_action("ACTION");
	react_activate(NULL);
	for (size_t i = 0; i < ITERATIONS_NUMBER; ++i) {
		react_start_action(action_code);
		x *= x;
		react_stop_action(action_code);
	}
	react_deactivate();

	std::cerr << x << std::endl;
	size_t stop_time = clock();

	double total_time = double(stop_time - start_time) / CLOCKS_PER_SEC * 1000;
	std::cerr << "Total time: " << total_time << "ms" << std::endl;
	double time_per_operation = total_time / ITERATIONS_NUMBER;
	std::cerr << "Time per operation: " << time_per_operation << "ms" << std::endl;
	int operations_per_sec = 1000 / time_per_operation;
	std::cerr << "Operations per sec: " << operations_per_sec << std::endl;

	return 0;
}
