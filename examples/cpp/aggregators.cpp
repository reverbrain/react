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

#include <thread>

#include "react/react.hpp"
#include "react/aggregators/recent_trees_aggregator.hpp"
#include "react/aggregators/histogram_aggregator.hpp"
#include "react/aggregators/category_filter_aggregator.hpp"
#include "react/aggregators/parent_call_tree_aggregator.hpp"
#include "react/utils.hpp"

int action_code = react_define_new_action("ACTION");

void run_recent_trees_aggregator_example() {
	react::recent_trees_aggregator_t aggregator(react::get_actions_set(), 3);
	react_activate(&aggregator);

	react_submit_progress();

	{
		react_start_action(action_code);
		react_stop_action(action_code);
		react_submit_progress();
	}

	{
		react_start_action(action_code);
		react_stop_action(action_code);
		react_start_action(action_code);
		react_stop_action(action_code);
		react_submit_progress();
	}

	{
		react_start_action(action_code);
		react_stop_action(action_code);
	}

	react_deactivate();

	print_json(aggregator);
}

void run_category_filter_aggregator_example() {
	auto important_trees_aggregator =
		std::make_shared<react::recent_trees_aggregator_t>(react::get_actions_set(), 3);

	auto unimportant_trees_aggregator =
		std::make_shared<react::recent_trees_aggregator_t>(react::get_actions_set(), 3);

	react::category_filter_aggregator_t<bool> category_filter_aggregator(
		react::get_actions_set(),
		std::make_shared<react::stat_extractor_t<bool>>("important")
	);

	category_filter_aggregator.add_category_aggregator(
		false, unimportant_trees_aggregator
	);

	category_filter_aggregator.add_category_aggregator(
		true, important_trees_aggregator
	);

	{
		react_activate(&category_filter_aggregator);
		react_add_stat_string("name", "first_tree");
		react_add_stat_bool("important", true);
		react_deactivate();
	}

	{
		react_activate(&category_filter_aggregator);
		react_add_stat_string("name", "second_tree");
		react_add_stat_bool("important", false);
		react_deactivate();
	}

	{
		react_activate(&category_filter_aggregator);
		react_add_stat_string("name", "third_tree");
		react_add_stat_bool("important", true);
		react_deactivate();
	}

	print_json(category_filter_aggregator);
}

void run_histogram_aggregator_example() {
	std::vector<int> ticks = {1, 2, 4, 8, 16, 32, 64, 128, 256};
	auto histogram_updater = std::make_shared<react::action_time_histogram_updater_t>(action_code);
	react::histogram_aggregator_t<react::histogram1D_t> histogram_aggregator(
		react::get_actions_set(), histogram_updater, ticks
	);

	{
		react_activate(&histogram_aggregator);
		react_start_action(action_code);
		react_stop_action(action_code);
		react_deactivate();
	}

	{
		react_activate(&histogram_aggregator);
		react_start_action(action_code);
		usleep(100);
		react_stop_action(action_code);
		react_deactivate();
	}

	{
		react_activate(&histogram_aggregator);
		react_start_action(action_code);
		react_stop_action(action_code);
		react_deactivate();
	}

	print_json(histogram_aggregator);
}

void run_batch_histogram_aggregator_example() {
	const int ACTIONS_NUMBER = 3;
	const int ITERATIONS_NUMBER = 100;

	react::batch_histogram_aggregator_t<react::histogram1D_t>
			batch_histogram_aggregator(react::get_actions_set());

	std::vector<int> actions;
	for (int i = 0; i < ACTIONS_NUMBER; ++i) {
		std::string action_name = "ACTION_" + std::to_string(static_cast<long long>(i));
		actions.push_back(react_define_new_action(action_name.c_str()));

		std::vector<int> ticks = {100, 200, 300, 400, 500, 600};

		batch_histogram_aggregator.add_histogram_aggregator(
			std::make_shared<react::action_time_histogram_updater_t>(actions[i]), ticks
		);
	}

	{
		for (size_t iteration = 0; iteration < ITERATIONS_NUMBER; ++iteration) {
			react_activate(&batch_histogram_aggregator);
			for (size_t i = 0; i < actions.size(); ++i) {
				react_start_action(actions[i]);
				usleep(rand() % 500);
				react_stop_action(actions[i]);
			}
			react_deactivate();
		}
	}

	print_json(batch_histogram_aggregator);
}

const int main_thread_action = react_define_new_action("MAIN_THREAD");
const int subthread_action = react_define_new_action("SUBTHREAD");

void subthread_worker(std::shared_ptr<react::aggregator_t> aggregator) {
	react_activate(&(*aggregator));

	react_start_action(subthread_action);
	react_stop_action(subthread_action);

	react_submit_progress();

	react_deactivate();
}

void run_subthread_aggregator_example() {
	react::recent_trees_aggregator_t aggregator(react::get_actions_set(), 2);

	react_activate(&aggregator);

	react_start_action(main_thread_action);

	std::thread thread(
		&subthread_worker,
		react::create_subthread_aggregator()
	);
	thread.join();

	react_stop_action(main_thread_action);

	react_start_action(main_thread_action);
	react_stop_action(main_thread_action);

	react_deactivate();

	print_json(aggregator);
}

int main() {
	run_recent_trees_aggregator_example();
	run_category_filter_aggregator_example();
	run_histogram_aggregator_example();
	run_batch_histogram_aggregator_example();
	run_subthread_aggregator_example();
	return 0;
}
