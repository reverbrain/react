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
#include "react/aggregators/filter_aggregator.hpp"
#include "react/utils.hpp"

int action_code = react_define_new_action("ACTION");

void run_recent_trees_aggregator_example() {
	react_context_t *context = react_activate();
	react::recent_trees_aggregator_t aggregator(3);

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

struct tag_filter_t : public react::filter_t {
	tag_filter_t(const std::string &tag): tag(tag) {}
	~tag_filter_t() {}

	bool operator () (const react::call_tree_t &call_tree) const {
		return (call_tree.has_stat("tag") && call_tree.get_stat<std::string>("tag") == tag);
	}

	std::string description() const {
		return "Filtering field <tag> with value <" + tag + ">";
	}

	std::string tag;
};

void run_filter_aggregator_example() {
	react::recent_trees_aggregator_t recent_trees_aggregator(3);
	react::filter_aggregator_t filter_aggregator(std::make_shared<tag_filter_t> ("important"), recent_trees_aggregator);

	{
		react_context_t *context = react_activate(&filter_aggregator);
		react_add_stat_string("tag", "important");
		react_deactivate(context);
	}

	{
		react_context_t *context = react_activate(&filter_aggregator);
		react_add_stat_string("tag", "not so important");
		react_deactivate(context);
	}

	{
		react_context_t *context = react_activate(&filter_aggregator);
		react_add_stat_string("tag", "important");
		react_deactivate(context);
	}

	print_json(filter_aggregator);
}

int main() {
	run_recent_trees_aggregator_example();
	run_filter_aggregator_example();
	return 0;
}
