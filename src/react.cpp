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

#define REACT_CPP

#include "react/react.hpp"

#include <stdexcept>

using namespace react;

static actions_set_t *actions_set = new actions_set_t();

int react_define_new_action(const char *action_name) {
	try {
		return actions_set->define_new_action(action_name);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
}

struct react_call_tree_t : concurrent_call_tree_t {};
struct react_call_tree_updater_t : call_tree_updater_t {};

static __thread call_tree_updater_t *thread_call_tree_updater = NULL;

#include <iostream>
#include <mutex>

int react_is_active() {
	return (thread_call_tree_updater != NULL);
}

react_call_tree_t *react_create_call_tree() {
	try {
		if (react_is_active()) {
			std::string error_message("Can't create react call tree: react is already active");
			std::cerr << error_message << std::endl;
			throw std::runtime_error(error_message);
		}

		return static_cast<react_call_tree_t*>(new concurrent_call_tree_t(*actions_set));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return NULL;
	}
}

int react_cleanup_call_tree(react_call_tree_t *call_tree) {
	try {
		delete (reinterpret_cast<concurrent_call_tree_t*> (call_tree));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

react_call_tree_updater_t *react_create_call_tree_updater(react_call_tree_t *call_tree) {
	try {
		if (react_is_active()) {
			std::string error_message("Can't create react call tree updater: react is already active");
			std::cerr << error_message << std::endl;
			throw std::runtime_error(error_message);
		}

		thread_call_tree_updater =
				new call_tree_updater_t(*reinterpret_cast<concurrent_call_tree_t*>(call_tree));
		return static_cast<react_call_tree_updater_t*>(thread_call_tree_updater);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return NULL;
	}
}

int react_cleanup_call_tree_updater(react_call_tree_updater_t *updater) {
	try {
		if (thread_call_tree_updater == NULL) {
			std::string error_message("Can't remove call tree updater: react is not active");
			std::cerr << error_message << std::endl;
			throw std::runtime_error(error_message);
		}

		thread_call_tree_updater = NULL;
		delete (reinterpret_cast<call_tree_updater_t*> (updater));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

int react_start_action(int action_code) {
	try {
		if (!react_is_active()) {
			return 0;
		}

		thread_call_tree_updater->start(action_code);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int react_stop_action(int action_code) {
	try {
		if (!react_is_active()) {
			return 0;
		}

		thread_call_tree_updater->stop(action_code);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

namespace react {

action_guard::action_guard(int action_code):
	m_action_guard(thread_call_tree_updater, action_code) {
}

action_guard::~action_guard() {}


const actions_set_t &get_actions_set() {
	return *actions_set;
}

void merge_call_tree(react_call_tree_t *react_call_tree, unordered_call_tree_t &unordered_call_tree) {
	reinterpret_cast<concurrent_call_tree_t*> (react_call_tree)->get_call_tree().merge_into(unordered_call_tree);
}

} // namespace react
