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

#include "react/react.hpp"
#include "react/aggregators/recent_trees_aggregator.hpp"
#include "react/aggregators/histogram_aggregator.hpp"
#include "react/aggregators/category_filter_aggregator.hpp"
#include "react/utils.hpp"

int action_code = react_define_new_action("ACTION");

void run_recent_trees_aggregator_example() {
	react_context_t *context = react_activate();
	react::recent_trees_aggregator_t aggregator(react::get_actions_set(), 3);

	aggregator.aggregate(react::get_react_context_call_tree(context));

	react_start_action(action_code);
	react_stop_action(action_code);
	aggregator.aggregate(react::get_react_context_call_tree(context));

	react_start_action(action_code);
	react_stop_action(action_code);
	react_start_action(action_code);
	react_stop_action(action_code);
	aggregator.aggregate(react::get_react_context_call_tree(context));

	react_start_action(action_code);
	react_stop_action(action_code);
	aggregator.aggregate(react::get_react_context_call_tree(context));

	print_json(aggregator);

	react_deactivate(context);
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
		react_context_t *context = react_activate(&category_filter_aggregator);
		react_add_stat_string("name", "first_tree");
		react_add_stat_bool("important", true);
		react_deactivate(context);
	}

	{
		react_context_t *context = react_activate(&category_filter_aggregator);
		react_add_stat_string("name", "second_tree");
		react_add_stat_bool("important", false);
		react_deactivate(context);
	}

	{
		react_context_t *context = react_activate(&category_filter_aggregator);
		react_add_stat_string("name", "third_tree");
		react_add_stat_bool("important", true);
		react_deactivate(context);
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
		react_context_t *context = react_activate(&histogram_aggregator);
		react_start_action(action_code);
		react_stop_action(action_code);
		react_deactivate(context);
	}

	{
		react_context_t *context = react_activate(&histogram_aggregator);
		react_start_action(action_code);
		usleep(100);
		react_stop_action(action_code);
		react_deactivate(context);
	}

	{
		react_context_t *context = react_activate(&histogram_aggregator);
		react_start_action(action_code);
		react_stop_action(action_code);
		react_deactivate(context);
	}

	print_json(histogram_aggregator);
}

int main() {
	run_recent_trees_aggregator_example();
	run_category_filter_aggregator_example();
	run_histogram_aggregator_example();
	return 0;
}
