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

#include "elliptics_react.h"
#include "elliptics_react.hpp"

#include "react/react.hpp"

#include <stdexcept>

using namespace react;

actions_set_t elliptics_actions;

#define DEFINE_ACTION(CODE, NAME) const int CODE = elliptics_actions.define_new_action(NAME)

#define DEFINE_DNET_ACTION(CODE, NAME) DEFINE_ACTION(ACTION_DNET_##CODE, NAME)

DEFINE_DNET_ACTION(PROCESS_CMD_RAW, "PROCESS_CMD_RAW");
DEFINE_DNET_ACTION(MONITOR_PROCESS_CMD, "MONITOR_PROCESS_CMD");
DEFINE_DNET_ACTION(PROCESS_INDEXES, "PROCESS_INDEXES");
DEFINE_DNET_ACTION(CMD_REVERSE_LOOKUP, "CMD_REVERSE_LOOKUP");
DEFINE_DNET_ACTION(CMD_JOIN_CLIENT, "CMD_JOIN_CLIENT");
DEFINE_DNET_ACTION(UPDATE_NOTIFY, "UPDATE_NOTIFY");
DEFINE_DNET_ACTION(NOTIFY_ADD, "NOTIFY_ADD");
DEFINE_DNET_ACTION(CMD_EXEC, "CMD_EXEC");
DEFINE_DNET_ACTION(CMD_STAT_COUNT, "CMD_STAT_COUNT");
DEFINE_DNET_ACTION(CMD_STATUS, "CMD_STATUS");
DEFINE_DNET_ACTION(CMD_AUTH, "CMD_AUTH");
DEFINE_DNET_ACTION(CMD_ITERATOR, "CMD_ITERATOR");
DEFINE_DNET_ACTION(CMD_BULK_READ, "CMD_BURK_READ");
DEFINE_DNET_ACTION(CMD_ROUTE_LIST, "CMD_ROUTE_LIST");

#define DEFINE_CACHE_ACTION(CODE, NAME) DEFINE_ACTION(ACTION_CACHE_##CODE, NAME)

DEFINE_ACTION(ACTION_CACHE, "CACHE");
DEFINE_CACHE_ACTION(WRITE, "WRITE");
DEFINE_CACHE_ACTION(READ, "READ");
DEFINE_CACHE_ACTION(REMOVE, "REMOVE");
DEFINE_CACHE_ACTION(LOOKUP, "LOOKUP");
DEFINE_CACHE_ACTION(LOCK, "LOCK");
DEFINE_CACHE_ACTION(FIND, "FIND");
DEFINE_CACHE_ACTION(ADD_TO_PAGE, "ADD_TO_PAGE");
DEFINE_CACHE_ACTION(RESIZE_PAGE, "RESIZE_PAGE");
DEFINE_CACHE_ACTION(SYNC_AFTER_APPEND, "SYNC_AFTER_APPEND");
DEFINE_CACHE_ACTION(WRITE_APPEND_ONLY, "WRITE_AFTER_APPEND_ONLY");
DEFINE_CACHE_ACTION(WRITE_AFTER_APPEND_ONLY, "WRITE_AFTER_APPEND_ONLY");
DEFINE_CACHE_ACTION(POPULATE_FROM_DISK, "POPULATE_FROM_DISK");
DEFINE_CACHE_ACTION(CLEAR, "CLEAR");
DEFINE_CACHE_ACTION(LIFECHECK, "LIFECHECK");
DEFINE_CACHE_ACTION(CREATE_DATA, "CREATE_DATA");
DEFINE_CACHE_ACTION(CAS, "CAS");
DEFINE_CACHE_ACTION(MODIFY, "MODIFY");
DEFINE_CACHE_ACTION(DECREASE_KEY, "DECREASE_KEY");
DEFINE_CACHE_ACTION(MOVE_RECORD, "MOVE_RECORD");
DEFINE_CACHE_ACTION(ERASE, "ERASE");
DEFINE_CACHE_ACTION(REMOVE_LOCAL, "REMOVE_LOCAL");
DEFINE_CACHE_ACTION(LOCAL_LOOKUP, "LOCAL_LOOKUP");
DEFINE_CACHE_ACTION(LOCAL_READ, "LOCAL_READ");
DEFINE_CACHE_ACTION(LOCAL_WRITE, "LOCAL_WRITE");
DEFINE_CACHE_ACTION(PREPARE_SYNC, "PREPARE_SYNC");
DEFINE_CACHE_ACTION(SYNC, "SYNC");
DEFINE_CACHE_ACTION(SYNC_BEFORE_OPERATION, "SYNC_BEFORE_OPERATION");
DEFINE_CACHE_ACTION(ERASE_ITERATE, "ERASE_ITERATE");
DEFINE_CACHE_ACTION(SYNC_ITERATE, "SYNC_ITERATE");
DEFINE_CACHE_ACTION(DNET_OPLOCK, "DNET_OPLOCK");
DEFINE_CACHE_ACTION(DESTRUCT, "DESTRUCT");


typedef concurrent_call_tree_t<call_tree_t> concurrent_call_tree;
typedef call_tree_updater_t<call_tree_t> call_tree_updater;

__thread call_tree_updater *thread_call_tree_updater = NULL;

#include <iostream>
#include <mutex>

int init_call_tree(void **call_tree) {
	try {
		*call_tree = new concurrent_call_tree(elliptics_actions);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int cleanup_call_tree(void *call_tree) {
	try {
		free((concurrent_call_tree*) call_tree);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}
	return 0;
}

int merge_call_tree(void *call_tree, void *elliptics_react_manager) {
	try {
		((elliptics_react_manager_t*) elliptics_react_manager)->add_tree(*((concurrent_call_tree*) call_tree));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -EFAULT;
	}

	return 0;
}

int init_empty_updater() {
	try {
		thread_call_tree_updater = new call_tree_updater();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int init_updater(void *call_tree) {
	try {
		thread_call_tree_updater = new call_tree_updater(*((concurrent_call_tree*) call_tree));
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -ENOMEM;
	}
	return 0;
}

int cleanup_updater() {
	try {
		if (thread_call_tree_updater) {
			delete thread_call_tree_updater;
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

elliptics_react_manager_t::elliptics_react_manager_t(): total_call_tree(elliptics_actions), last_call_tree(elliptics_actions) {

}

void elliptics_react_manager_t::add_tree(concurrent_call_tree &call_tree) {
	mutex.lock();
	auto call_tree_copy = call_tree.copy_time_stats_tree();
	call_tree_copy.merge_into(total_call_tree);
	last_call_tree.set(call_tree_copy);
	mutex.unlock();
}

const unordered_call_tree_t &elliptics_react_manager_t::get_total_call_tree() const {
	return total_call_tree;
}

const call_tree_t &elliptics_react_manager_t::get_last_call_tree() const {
	return last_call_tree;
}
