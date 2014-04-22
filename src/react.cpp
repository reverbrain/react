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
#include <iostream>
#include <mutex>

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

struct react_context_t {
	react_context_t(react::aggregator_t *aggregator):
		call_tree(*actions_set), updater(call_tree), aggregator(aggregator) {}

	concurrent_call_tree_t call_tree;
	call_tree_updater_t updater;
	react::aggregator_t *aggregator;
};

static __thread react_context_t *thread_react_context = NULL;

int react_is_active() {
	return thread_react_context != NULL;
}

react_context_t *react_activate(void *react_aggregator) {
	try {
		if (react_is_active()) {
			std::string error_message("Can't activate react: React is already active");
			throw std::runtime_error(error_message);
		}

		thread_react_context = new react_context_t(
					static_cast<react::aggregator_t*>(react_aggregator)
		);

		react::add_stat("complete", false);

		return thread_react_context;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return NULL;
	}
}

int react_deactivate(react_context_t *react_context) {
	try {
		if (!react_is_active()) {
			std::string error_message("Can't deactivate react: React is not active");
			throw std::runtime_error(error_message);
		}

		if (react_context != thread_react_context) {
			std::string error_message("Can't deactivate react: passed context doesn't match \
									  thread_local context");
			throw std::invalid_argument(error_message);
		}

		react::add_stat("complete", true);

		if (thread_react_context->aggregator) {
			thread_react_context->aggregator->aggregate(thread_react_context->call_tree.get_call_tree());
		}
		delete thread_react_context;
		thread_react_context = NULL;
	} catch (std::exception &e) {
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

		thread_react_context->updater.start(action_code);
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

		thread_react_context->updater.stop(action_code);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int react_add_stat_bool(const char *key, bool value) {
	try {
		if (!react_is_active()) {
			return 0;
		}

		react::add_stat(std::string(key), value);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int react_add_stat_int(const char *key, int value) {
	try {
		if (!react_is_active()) {
			return 0;
		}

		react::add_stat(std::string(key), value);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int react_add_stat_double(const char *key, double value) {
	try {
		if (!react_is_active()) {
			return 0;
		}

		react::add_stat(std::string(key), value);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int react_add_stat_string(const char *key, const char *value) {
	try {
		if (!react_is_active()) {
			return 0;
		}

		react::add_stat(std::string(key), value);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

int react_submit_progress() {
	try {
		if (!react_is_active()) {
			return 0;
		}

		if (thread_react_context->aggregator) {
			thread_react_context->aggregator->aggregate(thread_react_context->call_tree.get_call_tree());
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EINVAL;
	}
	return 0;
}

namespace react {

action_guard::action_guard(int action_code) {
	if (react_is_active()) {
		m_action_guard.reset(
					new action_guard_t(&thread_react_context->updater, action_code)
		);
	}
}

action_guard::~action_guard() {}

void react::action_guard::stop() {
	if (m_action_guard) {
		m_action_guard->stop();
	}
}


const actions_set_t &get_actions_set() {
	return *actions_set;
}

call_tree_t get_react_context_call_tree(react_context_t *react_context) {
	if (!react_context) {
		std::string error_message("Can't get react context call tree: passed context is null");
		throw std::invalid_argument(error_message);
	}

	return react_context->call_tree.copy_call_tree();
}

void add_stat(const std::string &key, const char *value) {
	add_stat_impl(key, react::stat_value_t(std::string(value)));
}

void add_stat_impl(const std::string &key, const react::stat_value_t &value) {
	thread_react_context->call_tree.get_call_tree().add_stat(key, value);
}

} // namespace react
