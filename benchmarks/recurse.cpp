#include <iostream>

#include "react/react.h"

size_t MAX_DEPTH = 1000000;
int action_code = react_define_new_action("ACTION");

void recurse(size_t depth) {
	if (depth > MAX_DEPTH) {
		return;
	}
	react_start_action(action_code);
	recurse(depth + 1);
	react_stop_action(action_code);
}

int main() {

	size_t start_time = clock();

	react_activate(NULL);
	recurse(0);
	react_deactivate();

	size_t stop_time = clock();

	double total_time = double(stop_time - start_time) / CLOCKS_PER_SEC * 1000;
	std::cerr << "Total time: " << total_time << "ms" << std::endl;
	double time_per_operation = total_time / MAX_DEPTH;
	std::cerr << "Time per operation: " << time_per_operation << "ms" << std::endl;
	int operations_per_sec = 1000 / time_per_operation;
	std::cerr << "Operations per sec: " << operations_per_sec << std::endl;

	return 0;
}

