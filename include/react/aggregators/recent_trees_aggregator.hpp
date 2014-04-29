/*
* 2014+ Copyright (c) Andrey Kashin <kashin.andrej@gmail.com>
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

#ifndef RECENT_TREES_AGGREGATOR_HPP
#define RECENT_TREES_AGGREGATOR_HPP

#include "aggregator.hpp"

namespace react {

class recent_trees_aggregator_t : public aggregator_t {
public:
	recent_trees_aggregator_t(const actions_set_t &actions_set,
							  size_t recent_list_size):
		aggregator_t(actions_set), recent_list_size(recent_list_size) {}
	~recent_trees_aggregator_t() {}

	void aggregate(const call_tree_t &call_tree) {
		recent_trees.push_back(call_tree);
		if (recent_trees.size() > recent_list_size) {
			recent_trees.pop_front();
		}
	}

	void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const {
		rapidjson::Value recent_trees_values(rapidjson::kArrayType);
		for (auto it = recent_trees.begin(); it != recent_trees.end(); ++it) {
			rapidjson::Value tree_value(rapidjson::kObjectType);
			it->to_json(tree_value, allocator);
			recent_trees_values.PushBack(tree_value, allocator);
		}
		value.AddMember("recent_trees_aggregator", recent_trees_values, allocator);
	}

private:
	size_t recent_list_size;
	std::list<call_tree_t> recent_trees;
};

} // namespace react

#endif // RECENT_TREES_AGGREGATOR_HPP
