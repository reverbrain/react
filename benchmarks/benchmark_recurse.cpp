#include "benchmarks.hpp"

#include <iostream>

#include "react/react.hpp"

const size_t MAX_DEPTH = 1000;
const size_t SAMPLES_NUMBER = 10;
const size_t USEC_SLEEP_TIME = 1000;

int action_code = react_define_new_action("ACTION");

void recurseVanilla(size_t depth) {
	if (depth > MAX_DEPTH) {
		return;
	}
	usleep(USEC_SLEEP_TIME);
	recurseVanilla(depth + 1);
}

void recurseStartStop(size_t depth) {
	if (depth > MAX_DEPTH) {
		return;
	}
	react_start_action(action_code);
	usleep(USEC_SLEEP_TIME);
	recurseStartStop(depth + 1);
	react_stop_action(action_code);
}

void recurseGuarded(size_t depth) {
	if (depth > MAX_DEPTH) {
		return;
	}
	react::action_guard guard(action_code);
	usleep(USEC_SLEEP_TIME);
	recurseGuarded(depth + 1);
}

BASELINE(Recurse, Vanilla, 1, SAMPLES_NUMBER)
{
	recurseVanilla(0);
}

BENCHMARK(Recurse, StartStop, 1, SAMPLES_NUMBER)
{
	react_activate(NULL);
	recurseStartStop(0);
	react_deactivate();
}

BENCHMARK(Recurse, Recurse, 1, SAMPLES_NUMBER)
{
	react_activate(NULL);
	recurseGuarded(0);
	react_deactivate();
}
