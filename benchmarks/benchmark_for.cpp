#include "benchmarks.hpp"

#include <iostream>

#include "react/react.hpp"

const size_t FOR_ITERATIONS_NUMBER = 1000;
const size_t SAMPLES_NUMBER = 10;
const size_t USEC_SLEEP_TIME = 1000;

BASELINE(ForLoop, Vanilla, 1, SAMPLES_NUMBER)
{
	for(size_t i = 0; i < FOR_ITERATIONS_NUMBER; ++i) {
		usleep(USEC_SLEEP_TIME);
	}
}

BENCHMARK(ForLoop, StartStop, 1, SAMPLES_NUMBER)
{
	int action_code = react_define_new_action("ACTION");
	react_activate();
	for(size_t i = 0; i < FOR_ITERATIONS_NUMBER; ++i) {
		react_start_action(action_code);
		usleep(USEC_SLEEP_TIME);
		react_stop_action(action_code);
	}
	react_deactivate();
}

BENCHMARK(ForLoop, Guarded, 1, SAMPLES_NUMBER)
{
	int action_code = react_define_new_action("ACTION");
	react_activate();
	for(size_t i = 0; i < FOR_ITERATIONS_NUMBER; ++i) {
		react::action_guard guard(action_code);
		usleep(USEC_SLEEP_TIME);
	}
	react_deactivate();
}
