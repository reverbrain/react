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

#ifndef PARENT_CALL_TREE_AGGREGATOR_HPP
#define PARENT_CALL_TREE_AGGREGATOR_HPP

#include "aggregator.hpp"

namespace react {

class parent_call_tree_aggregator_t : public aggregator_t {
public:
	parent_call_tree_aggregator_t(const actions_set_t &actions_set,
								  call_tree_t &parent_call_tree,
								  call_tree_t::p_node_t parent_node):
		aggregator_t(actions_set),
		parent_call_tree(parent_call_tree),
		parent_node(parent_node) {}
	~parent_call_tree_aggregator_t() {}

	void aggregate(const call_tree_t &call_tree) {
		call_tree.merge_into(parent_node, parent_call_tree);
	}

	void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const {
		rapidjson::Value aggregator_value(rapidjson::kObjectType);
		int action_code = parent_call_tree.get_node_action_code(parent_node);
		std::string action_name = parent_call_tree.get_actions_set().get_action_name(action_code);
		aggregator_value.AddMember("parent_action", action_name, allocator);
		value.AddMember("parent_call_tree_aggregator", aggregator_value, allocator);
	}

private:
	call_tree_t &parent_call_tree;
	call_tree_t::p_node_t parent_node;
};

} // namespace react
#endif // PARENT_CALL_TREE_AGGREGATOR_HPP
