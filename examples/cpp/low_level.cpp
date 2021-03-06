/*
* 2013+ Copyright (c) Andrey Kashin <kashin.andrej@gmail.com>
* All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*/

#ifndef _GLIBCXX_USE_NANOSLEEP
#define _GLIBCXX_USE_NANOSLEEP
#endif

#ifndef _GLIBCXX_USE_CLOCK_REALTIME
#define _GLIBCXX_USE_CLOCK_REALTIME
#endif

#include <thread>
#include <chrono>

#include "react/react.hpp"
#include "react/utils.hpp"

using namespace react;

actions_set_t actions_set; // Define set of actions that will be monitored
const int ACTION_READ = actions_set.define_new_action("READ");
const int ACTION_FIND = actions_set.define_new_action("FIND");
const int ACTION_LOAD_FROM_DISK = actions_set.define_new_action("LOAD FROM DISK");
const int ACTION_READ_FROM_DISK = actions_set.define_new_action("READ FROM DISK");
const int ACTION_PUT_INTO_CACHE = actions_set.define_new_action("PUT INTO CACHE");
const int ACTION_LOAD_FROM_CACHE = actions_set.define_new_action("LOAD FROM CACHE");

call_tree_updater_t updater; // Updater for gathering of statistics.

// Defining stub functions
bool find_record() {
	std::this_thread::sleep_for( std::chrono::microseconds(10) );
	return (rand() % 4) == 0;
}

std::string read_from_disk() {
	action_guard_t read_from_disk_guard(&updater, ACTION_READ_FROM_DISK);

	std::this_thread::sleep_for( std::chrono::microseconds(1000) );
	return "DISK";
}

void put_into_cache(std::string data) {
	action_guard_t put_into_cache_guard(&updater, ACTION_PUT_INTO_CACHE);

	std::this_thread::sleep_for( std::chrono::microseconds(50) );
}

std::string load_from_cache() {
	action_guard_t load_from_cache_guard(&updater, ACTION_LOAD_FROM_CACHE);

	std::this_thread::sleep_for( std::chrono::microseconds(25) );
	return "CACHE";
}

std::string cache_read() {
	action_guard_t read_guard(&updater, ACTION_READ); // Creates new guard and starts action which will be stopped on guard's destructor

	std::string data;

	updater.start(ACTION_FIND); // Starts new action which will be inner to ACTION_READ
	bool found = find_record();
	updater.stop(ACTION_FIND);

	if (!found) {
		action_guard_t load_from_disk_guard(&updater, ACTION_LOAD_FROM_DISK);

		data = read_from_disk();
		put_into_cache(data);
		return data; // Here all action guards are destructed and actions are correctly finished
	}
	data = load_from_cache();

	return data;
}

const int ITERATIONS_NUMBER = 10;

void run_example() {
	std::cout << "Running cache read " << ITERATIONS_NUMBER << " times" << std::endl;

	react::stream_aggregator_t aggregator(std::cout);

	for (int i = 0; i < ITERATIONS_NUMBER; ++i) {
		concurrent_call_tree_t call_tree(actions_set); // Call tree for storing statistics.
		updater.set_call_tree(call_tree);
		std::string data = cache_read();
		aggregator.aggregate(call_tree.get_call_tree());
		updater.reset_call_tree();
	}
}

int main() {
	run_example();
	return 0;
}
