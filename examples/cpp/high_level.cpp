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

#include <thread>

#include <react/react.hpp>

const int ACTION_READ = react_define_new_action("READ");
const int ACTION_FIND = react_define_new_action("FIND");
const int ACTION_LOAD_FROM_DISK = react_define_new_action("LOAD FROM DISK");
const int ACTION_READ_FROM_DISK = react_define_new_action("READ FROM DISK");
const int ACTION_PUT_INTO_CACHE = react_define_new_action("PUT INTO CACHE");
const int ACTION_LOAD_FROM_CACHE = react_define_new_action("LOAD FROM CACHE");

using react::action_guard;

// Defining stub functions
bool find_record() {
	std::this_thread::sleep_for( std::chrono::microseconds(10) );
	return (rand() % 4) == 0;
}

std::string read_from_disk() {
	action_guard guard(ACTION_READ_FROM_DISK);

	std::this_thread::sleep_for( std::chrono::microseconds(1000) );
	return "DISK";
}

void put_into_cache(std::string data) {
	action_guard put_into_cache_guard(ACTION_PUT_INTO_CACHE);

	std::this_thread::sleep_for( std::chrono::microseconds(50) );
}

std::string load_from_cache() {
	action_guard load_from_cache_guard(ACTION_LOAD_FROM_CACHE);

	std::this_thread::sleep_for( std::chrono::microseconds(25) );
	return "CACHE";
}

std::string cache_read() {
	action_guard read_guard(ACTION_READ); // Creates new guard and starts action which will be stopped on guard's destructor

	std::string data;

	react_start_action(ACTION_FIND); // Starts new action which will be inner to ACTION_READ
	bool found = find_record();
	react_stop_action(ACTION_FIND);

	if (!found) {
		action_guard load_from_disk_guard(ACTION_LOAD_FROM_DISK);

		data = read_from_disk();
		put_into_cache(data);
		return data; // Here all action guards are destructed and actions are correctly finished
	}
	data = load_from_cache();

	return data;
}

template<typename TreeType>
void print_json(const TreeType &time_stats) {
	rapidjson::Document doc;
	doc.SetObject();
	auto &allocator = doc.GetAllocator();

	time_stats.to_json(doc, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string result = buffer.GetString();
	std::cout << result << std::endl;
}

const int ITERATIONS_NUMBER = 1000;

void run_example() {
	std::cout << "Running cache read " << ITERATIONS_NUMBER << " times" << std::endl;

	react::unordered_call_tree_t total_time_stats(react::get_actions_set());

	for (int i = 0; i < ITERATIONS_NUMBER; ++i) {
		react_call_tree_t *call_tree = react_create_call_tree();
		react_call_tree_updater_t *call_tree_updater = react_create_call_tree_updater(call_tree);

		std::string data = cache_read();

		react::merge_call_tree(call_tree, total_time_stats);
		react_cleanup_call_tree_updater(call_tree_updater);
		react_cleanup_call_tree(call_tree);
	}

	print_json(total_time_stats);
}

int main() {
	run_example();
	return 0;
}
