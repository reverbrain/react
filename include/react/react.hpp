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

/*!
 * \file react.hpp
 * \brief Tools for time monitoring
 *
 * This file contains tools for detailed time monitoring,
 * that allow you to gather time statistics in call tree manner.
 * This way you can see time consumed by each action and
 * it's distribution between other inner actions, which perfectly reveals bottlenecks.
 *
 * Example of simple monitoring:
 * \code
 *      actions_set_t actions_set; // Define set of actions that will be monitored
 *      const int ACTION_READ = actions_set.define_new_action("READ");
 *      ...
 *      const int ACTION_FIND = actions_set.define_new_action("FIND");
 *      concurrent_time_stats_tree_t time_stats(actions_set); // Call tree for storing statistics.
 *      time_stats_updater_t updater(time_stats); // Updater for gathering of statistics.
 *
 *      void cache_read(...) {
 *          action_guard read_guard(&updater, ACTION_READ); // Creates new guard and starts action which will be stopped on guard's destructor
 *          updater.start(ACTION_FIND); // Starts new action which will be inner to ACTION_READ
 *          found = find_record(...);
 *          updater.stop(ACTION_FIND);
 *          if (!found) {
 *              action_guard read_from_disk_guard(&updater, ACTION_READ_FROM_DISK);
 *              updater.start(ACTION_LOAD_FROM_DISK);
 *              data = read_from_disk(...);
 *              updater.stop(ACTION_LOAD_FROM_DISK);
 *              updater.start(ACTION_PUT_INTO_CACHE);
 *              put_into_cache(...);
 *              updater.stop(ACTION_PUT_INTO_CACHE);
 *              return data; // Here all action guards are destructed and actions are correctly finished
 *          }
 *          updater.start(ACTION_LOAD_FROM_CACHE);
 *          data = load_from_cache(...);
 *          updater.stop(ACTION_LOAD_FROM_CACHE);
 *          return data;
 *      }
 * \endcode
 * This code with build such call tree:
 *
 * - ACTION_READ
 *      - ACTION_FIND
 *      - ACTION_READ_FROM_DISK
 *          - ACTION_LOAD_FROM_DISK
 *          - ACTION_PUT_INTO_CACHE
 *      - ACTION_LOAD_FROM_CACHE
 */

#ifndef REACT_HPP
#define REACT_HPP

#include <stack>
#include <chrono>
#include <unordered_map>
#include <map>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <mutex>
#include <sstream>

#include "call_tree.hpp"
#include "updater.hpp"

#endif // REACT_HPP
