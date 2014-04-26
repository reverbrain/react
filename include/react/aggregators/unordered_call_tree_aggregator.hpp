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

#ifndef UNORDERED_CALL_TREE_AGGREGATOR_HPP
#define UNORDERED_CALL_TREE_AGGREGATOR_HPP

#include "aggregator.hpp"
#include "../call_tree.hpp"

namespace react {

class unordered_call_tree_t : public call_tree_base_t<unordered_node_t> {
	typedef call_tree_base_t<unordered_node_t> Base;

public:
	unordered_call_tree_t(const actions_set_t &actions_set): Base(actions_set) {}

	/*!
	 * \brief frees memory consumed by call tree
	 */
	~unordered_call_tree_t() {}

	/*!
	 * \brief Sets total time consumed by action represented by \a node
	 * \param node Action's node
	 * \param time New time value
	 */
	void set_node_time(p_node_t node, int64_t time) {
		nodes[node].time = time;
	}

	/*!
	 * \brief Increments total time consumed by action represented by \a node
	 * \param node Action's node
	 * \param delta Value by which time will be incremented
	 */
	void inc_node_time(p_node_t node, int64_t delta) {
		nodes[node].time += delta;
	}

	/*!
	 * \brief Returns total time consumed by action represented by \a node
	 * \param node Action's node
	 * \return Time consumed by action
	 */
	int64_t get_node_time(p_node_t node) const {
		return nodes[node].time;
	}

	/*!
	 * \brief Sets total calls number of action represented by \a node
	 * \param node Action's node
	 * \param time New calls number
	 */
	void set_node_calls_number(p_node_t node, int64_t calls_number) {
		nodes[node].calls_number = calls_number;
	}

	/*!
	 * \brief Increments total calls number of action represented by \a node
	 * \param node Action's node
	 */
	void inc_node_calls_number(p_node_t node) {
		++nodes[node].calls_number;
	}

	/*!
	 * \brief Returns total calls number of action represented by \a node
	 * \param node Action's node
	 * \return Time calls number of action
	 */
	int64_t get_node_calls_number(p_node_t node) const {
		return nodes[node].calls_number;
	}

	/*!
	 * \brief Checks whether node has child with \a action_code
	 * \param node Target node
	 * \param action_code Child's action code
	 * \return whether \a node has child with \a action_code
	 */
	bool node_has_link(p_node_t node, int action_code) const {
		return nodes[node].links.find(action_code) != nodes[node].links.end();
	}

	/*!
	 * \brief Gets node's child with \a action_code
	 * \param node Target node
	 * \param action_code Child's action code
	 * \return Pointer to child with \a action_code
	 */
	p_node_t get_node_link(p_node_t node, int action_code) const {
		return nodes[node].links.at(action_code);
	}

	/*!
	 * \brief Adds new child to \a node with \a action_code
	 * \param node Target node
	 * \param action_code Child's action code
	 * \return Pointer to newly created child of \a node with \a action_code
	 */
	p_node_t add_new_link(p_node_t node, int action_code) {
		p_node_t action_node = new_node(action_code);
		nodes[node].links.insert(std::make_pair(action_code, action_node));
		return action_node;
	}

	/*!
	 * \brief Adds new child to \a node with \a action_code if it's missing
	 * \param node Target node
	 * \param action_code Child's action code
	 * \return Pointer to child of \a node with \a action_code
	 */
	p_node_t add_new_link_if_missing(p_node_t node, int action_code) {
		auto link = nodes[node].links.find(action_code);
		if (link == nodes[node].links.end()) {
			return add_new_link(node, action_code);
		}
		return link->second;
	}

	/*!
	* \brief Merges this tree into \a another_tree
	* \param another_tree Tree where current tree will be merged in
	*/
	void merge_into(unordered_call_tree_t& another_tree) const {
		merge_into(root, another_tree.root, another_tree);
	}

	/*!
	* \brief Merges this tree with \a another_tree
	* \param another_tree Tree which will be merged into current tree
	*/
	void merge_with(const call_tree_t& another_tree) {
		merge_with(root, another_tree.root, another_tree);
	}

	using Base::to_json;

private:
	/*!
	 * \internal
	 *
	 * \brief Recursively converts subtree to json
	 * \param current_node Node which subtree will be converted
	 * \param stat_value Json node for writing
	 * \param allocator Json allocator
	 * \return Modified json node
	 */
	rapidjson::Value& to_json(p_node_t current_node, rapidjson::Value &stat_value,
							  rapidjson::Document::AllocatorType &allocator) const {
		if (current_node != root) {
			stat_value.AddMember("name", actions_set.get_action_name(get_node_action_code(current_node)).c_str(), allocator);
			stat_value.AddMember("time", (int64_t) get_node_time(current_node), allocator);
			stat_value.AddMember("calls", (int64_t) get_node_calls_number(current_node), allocator);
		}

		if (!nodes[current_node].links.empty()) {
			rapidjson::Value subtree_actions(rapidjson::kArrayType);

			for (auto it = nodes[current_node].links.begin(); it != nodes[current_node].links.end(); ++it) {
				p_node_t next_node = it->second;
				rapidjson::Value subtree_value(rapidjson::kObjectType);
				to_json(next_node, subtree_value, allocator);
				subtree_actions.PushBack(subtree_value, allocator);
			}

			stat_value.AddMember("actions", subtree_actions, allocator);
		}

		return stat_value;
	}

	/*!
	 * \internal
	 *
	 * \brief Recursively merges \a lhs_node into \a rhs_node
	 * \param lhs_node Node which will be merged
	 * \param rhs_node Node into which merge will be applied
	 * \param rhs_tree Tree into which merge will be applied
	 */
	void merge_into(p_node_t lhs_node, p_node_t rhs_node, unordered_call_tree_t& rhs_tree) const {
		rhs_tree.set_node_time(rhs_node, rhs_tree.get_node_time(rhs_node) + get_node_time(lhs_node));
		rhs_tree.set_node_calls_number(rhs_node, rhs_tree.get_node_calls_number(rhs_node) + get_node_calls_number(lhs_node));

		for (auto it = nodes[lhs_node].links.begin(); it != nodes[lhs_node].links.end(); ++it) {
			int action_code = it->first;
			p_node_t lhs_next_node = it->second;
			if (!rhs_tree.node_has_link(rhs_node, action_code)) {
				rhs_tree.add_new_link(rhs_node, action_code);
			}
			p_node_t rhs_next_node = rhs_tree.get_node_link(rhs_node, action_code);
			merge_into(lhs_next_node, rhs_next_node, rhs_tree);
		}
	}

	/*!
	 * \internal
	 *
	 * \brief Recursively merges \a rhs_node into \a lhs_node
	 * \param lhs_node Node into which merge will be applied
	 * \param rhs_node Node which will be merged
	 * \param rhs_tree Tree that will be merged
	 */
	void merge_with(p_node_t lhs_node, p_node_t rhs_node, const call_tree_t& rhs_tree) {
		int64_t time_delta = rhs_tree.get_node_stop_time(rhs_node) - rhs_tree.get_node_start_time(rhs_node);
		set_node_time(lhs_node, get_node_time(lhs_node) + time_delta);
		set_node_calls_number(lhs_node, get_node_calls_number(lhs_node) + 1);

		for (auto it = rhs_tree.get_node_links(rhs_node).begin(); it != rhs_tree.get_node_links(rhs_node).end(); ++it) {
			int action_code = it->first;
			p_node_t rhs_next_node = it->second;
			if (!node_has_link(lhs_node, action_code)) {
				add_new_link(lhs_node, action_code);
			}
			p_node_t lhs_next_node = get_node_link(lhs_node, action_code);
			merge_with(lhs_next_node, rhs_next_node, rhs_tree);
		}
	}
};

class unordered_call_tree_aggregator_t : public aggregator_t {
public:
	unordered_call_tree_aggregator_t(const actions_set_t &actions_set):
		aggregator_t(actions_set), unordered_call_tree(actions_set) {}
	~unordered_call_tree_aggregator_t() {}

	void aggregate(const call_tree_t &call_tree) {
		unordered_call_tree.merge_with(call_tree);
	}

	void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const {
		rapidjson::Value unordered_call_tree_value(rapidjson::kObjectType);
		unordered_call_tree.to_json(unordered_call_tree_value, allocator);
		value.AddMember("unordered_call_tree", unordered_call_tree_value, allocator);
	}

private:
	unordered_call_tree_t unordered_call_tree;
};

} // namespace react

#endif // UNORDERED_CALL_TREE_AGGREGATOR_HPP
