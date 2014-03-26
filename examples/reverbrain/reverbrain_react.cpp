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

#ifndef REVERBRAIN_REACT_CPP
#define REVERBRAIN_REACT_CPP

#include "react/react.hpp"

#include <stdexcept>

using namespace react;

void *reverbrain_actions_ptr = (void*) new actions_set_t();
static actions_set_t *reverbrain_actions = (actions_set_t*) reverbrain_actions_ptr;

#include "reverbrain_react.hpp"

typedef concurrent_call_tree_t<call_tree_t> concurrent_call_tree;
typedef call_tree_updater_t<call_tree_t> call_tree_updater;

__thread void *thread_call_tree_updater_ptr = NULL;
static __thread call_tree_updater *thread_call_tree_updater = NULL;

#include <iostream>
#include <mutex>

int init_call_tree(void **call_tree) {
	try {
		*call_tree = new concurrent_call_tree(*reverbrain_actions);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int cleanup_call_tree(void *call_tree) {
	try {
		delete reinterpret_cast<concurrent_call_tree*>(call_tree);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

int merge_call_tree(void *call_tree, void *reverbrain_react_manager) {
	if (!reverbrain_react_manager) {
		return 0;
	}

	try {
		((reverbrain_react_manager_t*) reverbrain_react_manager)->add_tree(*((concurrent_call_tree*) call_tree));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}

	return 0;
}

int init_empty_updater() {
	try {
		thread_call_tree_updater_ptr = (void*) new call_tree_updater();
		thread_call_tree_updater = (call_tree_updater*) thread_call_tree_updater_ptr;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int init_updater(void *call_tree) {
	try {
		thread_call_tree_updater_ptr = (void*) new call_tree_updater(*((concurrent_call_tree*) call_tree));
		thread_call_tree_updater = (call_tree_updater*) thread_call_tree_updater_ptr;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int cleanup_updater() {
	try {
		if (thread_call_tree_updater) {
			delete (call_tree_updater*) thread_call_tree_updater_ptr;
			thread_call_tree_updater_ptr = NULL;
			thread_call_tree_updater = NULL;
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

int set_current_call_tree(void *call_tree) {
	try {
		thread_call_tree_updater->set_time_stats_tree(*((concurrent_call_tree*) call_tree));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

int call_tree_is_set() {
	try {
		if (!thread_call_tree_updater) {
			return false;
		}
		return thread_call_tree_updater->has_time_stats_tree();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

int start_action(int action_code) {
	if (!thread_call_tree_updater) {
		return 0;
	}

	try {
		thread_call_tree_updater->start(action_code);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int stop_action(int action_code) {
	if (!thread_call_tree_updater) {
		return 0;
	}

	try {
		thread_call_tree_updater->stop(action_code);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

action_guard make_action_guard(int action_code) {
	return action_guard(thread_call_tree_updater, action_code);
}

reverbrain_react_manager_t::reverbrain_react_manager_t(): total_call_tree(*reverbrain_actions), last_call_tree(*reverbrain_actions) {

}

void reverbrain_react_manager_t::add_tree(concurrent_call_tree &call_tree) {
	mutex.lock();
	auto call_tree_copy = call_tree.copy_time_stats_tree();
	call_tree_copy.merge_into(total_call_tree);
	last_call_tree.set(call_tree_copy);
	mutex.unlock();
}

const unordered_call_tree_t &reverbrain_react_manager_t::get_total_call_tree() const {
	return total_call_tree;
}

const call_tree_t &reverbrain_react_manager_t::get_last_call_tree() const {
	return last_call_tree;
}

#endif // REVERBRAIN_REACT_CPP
