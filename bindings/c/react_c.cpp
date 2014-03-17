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

#include "react_c.h"
#include "react/react.hpp"

#include <stdexcept>

using namespace react;

typedef concurrent_call_tree_t<unordered_call_tree_t> concurrent_time_stats_tree_t;
typedef call_tree_updater_t<unordered_call_tree_t> time_stats_updater_t;

__thread time_stats_updater_t *thread_time_stats_updater;

int init_time_stats_tree(void **time_stats_tree, void *actions) {
	try {
		*time_stats_tree = new concurrent_time_stats_tree_t(*((actions_set_t*) actions));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int cleanup_time_stats_tree(void **time_stats_tree) {
	try {
		free((concurrent_time_stats_tree_t*) *time_stats_tree);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

int init_updater(void *time_stats_tree) {
	try {
		if (!thread_time_stats_updater) {
			thread_time_stats_updater = new time_stats_updater_t(
						*(concurrent_time_stats_tree_t*) time_stats_tree);
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int start_action(void *time_stats_tree, int action_code) {
	int err = 0;
	err = init_updater(time_stats_tree);
	if (err) {
		return err;
	}

	try {
		thread_time_stats_updater->start(action_code);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int stop_action(void *time_stats_tree, int action_code) {
	int err = 0;
	err = init_updater(time_stats_tree);
	if (err) {
		return err;
	}

	try {
		thread_time_stats_updater->stop(action_code);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int get_time_stats(void *time_stats_tree, char **time_stats, size_t *size) {
	static char *static_time_stats = NULL;
	static size_t static_size = 0;

	try {
		rapidjson::Document doc;
		doc.SetObject();
		auto &allocator = doc.GetAllocator();
		rapidjson::Value total_stats(rapidjson::kObjectType);

		((concurrent_time_stats_tree_t*) time_stats_tree)->copy_time_stats_tree().to_json(total_stats, allocator);

		doc.AddMember("time_stats", total_stats, allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		std::string result = buffer.GetString();

		if (static_size < result.length() + 1) {
			static_time_stats = (char*) realloc(static_time_stats, result.length() + 1);
		}
		static_size = result.length();
		strcpy(static_time_stats, result.c_str());
		*time_stats = static_time_stats;
		*size = static_size;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}



int get_time_stats(void *time_stats_tree, rapidjson::Value &stat_value, rapidjson::Document::AllocatorType &allocator)
{
	(void) ((concurrent_time_stats_tree_t*) time_stats_tree)->copy_time_stats_tree().to_json(stat_value, allocator);
	return 0;
}
