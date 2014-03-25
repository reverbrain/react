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

#ifndef call_tree_hpp
#define call_tree_hpp

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "actions_set.hpp"

namespace react {

template<typename Container>
struct node_t {
	/*!
	 * \brief Pointer to node type
	 */
	typedef size_t pointer;

	/*!
	 * \brief Initializes node with \a action_code and zero consumed time and calls number
	 * \param action_code Action code of the node
	 */
	node_t(int action_code): action_code(action_code), time(0), calls_number(0) {}

	/*!
	 * \brief action which this node represents
	 */
	int action_code;

	/*!
	 * \brief total time consumed in this node
	 */
	long long int time;

	/*!
	 * \brief number of calls in this node
	 */
	long long int calls_number;

	/*!
	 * \brief Child nodes, actions that happen inside this action
	 */
	Container links;
};

typedef node_t< std::unordered_map<int, size_t> > unordered_node_t;
typedef node_t< std::vector< std::pair<int, size_t> > > ordered_node_t;

class unordered_call_tree_t;

/*!
 * \brief Stores call tree.
 *
 * Each node of the tree represents information about single action:
 * - Action code
 * - Total time consumed during this action
 */
template<typename NodeType>
class call_tree_base_t {
public:
	typedef NodeType                   node_t;
	typedef typename NodeType::pointer p_node_t;

	/*!
	 * \brief Value for representing null pointer
	 */
	static const p_node_t NO_NODE = -1;

	/*!
	 * \brief initializes call tree with single root node and action set
	 * \param actions_set Set of available actions for monitoring in call tree
	 */
	call_tree_base_t(const actions_set_t &actions_set): actions_set(actions_set) {
		root = new_node(-1);
	}

	/*!
	 * \brief frees memory consumed by call tree
	 */
	virtual ~call_tree_base_t() {}

	virtual void clear() {
		nodes.clear();
		root = new_node(-1);
	}

	virtual void set(const call_tree_base_t<NodeType> &rhs) {
		nodes = rhs.nodes;
		root = rhs.root;
	}

	/*!
	 * \brief Converts call tree to json
	 * \param stat_value Json node for writing
	 * \param allocator Json allocator
	 * \return Modified json node
	 */
	rapidjson::Value& to_json(rapidjson::Value &stat_value,
							  rapidjson::Document::AllocatorType &allocator) const {
		return to_json(root, stat_value, allocator);
	}

	/*!
	 * \brief Merges this tree into \a another_tree
	 * \param another_tree Tree where current tree will be merged in
	 */
	void merge_into(unordered_call_tree_t& another_tree) const;

	/*!
	 * \brief Substracts time stats of this tree from \a another tree
	 * \param another_tree Tree from which this tree will be substracted
	 */
	void substract_from(unordered_call_tree_t& another_tree) const;

	/*!
	 * \brief Returns an action code for \a node
	 * \param node Target node
	 * \return Action code of the target node
	 */
	int get_node_action_code(p_node_t node) const {
		return nodes[node].action_code;
	}

	/*!
	 * \brief Sets total time consumed by action represented by \a node
	 * \param node Action's node
	 * \param time New time value
	 */
	void set_node_time(p_node_t node, long long time) {
		nodes[node].time = time;
	}

	/*!
	 * \brief Increments total time consumed by action represented by \a node
	 * \param node Action's node
	 * \param delta Value by which time will be incremented
	 */
	void inc_node_time(p_node_t node, long long delta) {
		nodes[node].time += delta;
	}

	/*!
	 * \brief Returns total time consumed by action represented by \a node
	 * \param node Action's node
	 * \return Time consumed by action
	 */
	long long int get_node_time(p_node_t node) const {
		return nodes[node].time;
	}

	/*!
	 * \brief Sets total calls number of action represented by \a node
	 * \param node Action's node
	 * \param time New calls number
	 */
	void set_node_calls_number(p_node_t node, long long calls_number) {
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
	long long int get_node_calls_number(p_node_t node) const {
		return nodes[node].calls_number;
	}

	/*!
	 * \brief Returns actions set monitored by this tree
	 * \return Actions set monitored by this tree
	 */
	const actions_set_t& get_actions_set() const {
		return actions_set;
	}

	/*!
	 * \brief Root of the call tree
	 */
	p_node_t root;

protected:
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
	 * \param rhs_node
	 * \param rhs_tree
	 */
	void merge_into(p_node_t lhs_node, p_node_t rhs_node, unordered_call_tree_t& rhs_tree) const;

	/*!
	 * \internal
	 *
	 * \brief Recursively substracts \a lhs_node from \a rhs_node
	 * \param lhs_node Node which will be substracted
	 * \param rhs_node
	 * \param rhs_tree
	 */
	void substract_from(p_node_t lhs_node, p_node_t rhs_node, unordered_call_tree_t& rhs_tree) const;

	/*!
	 * \internal
	 *
	 * \brief Allocates space for new node
	 * \param action_code
	 * \return Pointer to newly created node
	 */
	p_node_t new_node(int action_code) {
		nodes.emplace_back(action_code);
		return nodes.size() - 1;
	}
	/*!
	 * \brief Tree nodes
	 */
	std::vector<node_t> nodes;

	/*!
	 * \brief Available actions for monitoring
	 */
	const actions_set_t &actions_set;
};

class unordered_call_tree_t : public call_tree_base_t<unordered_node_t> {
	typedef call_tree_base_t<unordered_node_t> Base;

public:
	unordered_call_tree_t(const actions_set_t &actions_set): Base(actions_set) {}

	/*!
	 * \brief frees memory consumed by call tree
	 */
	~unordered_call_tree_t() {}

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
	 * \brief Calculates time differences between this tree and \a another tree
	 * \param another_tree Tree which will be substracted from this tree
	 */
	unordered_call_tree_t diff_from(unordered_call_tree_t& another_tree) const {
		unordered_call_tree_t diff_tree = *this;
		another_tree.substract_from(diff_tree);
		return std::move(diff_tree);
	}

	using Base::to_json;
	using Base::merge_into;

private:
};

template<typename NodeType>
void call_tree_base_t<NodeType>::merge_into(unordered_call_tree_t& another_tree) const {
	merge_into(root, another_tree.root, another_tree);
}

template<typename NodeType>
void call_tree_base_t<NodeType>::substract_from(unordered_call_tree_t& another_tree) const {
	return substract_from(root, another_tree.root, another_tree);
}

template<typename NodeType>
void call_tree_base_t<NodeType>::merge_into(p_node_t lhs_node, p_node_t rhs_node, unordered_call_tree_t& rhs_tree) const {
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

template<typename NodeType>
void call_tree_base_t<NodeType>::substract_from(p_node_t lhs_node, p_node_t rhs_node, unordered_call_tree_t& rhs_tree) const {
	rhs_tree.set_node_time(rhs_node, rhs_tree.get_node_time(rhs_node) - get_node_time(lhs_node));
	rhs_tree.set_node_calls_number(rhs_node, rhs_tree.get_node_calls_number(rhs_node) - get_node_calls_number(lhs_node));

	for (auto it = nodes[lhs_node].links.begin(); it != nodes[lhs_node].links.end(); ++it) {
		int action_code = it->first;
		p_node_t lhs_next_node = it->second;
		if (!rhs_tree.node_has_link(rhs_node, action_code)) {
			rhs_tree.add_new_link(rhs_node, action_code);
		}
		p_node_t rhs_next_node = rhs_tree.get_node_link(rhs_node, action_code);
		substract_from(lhs_next_node, rhs_next_node, rhs_tree);
	}
}

class call_tree_t : public call_tree_base_t<ordered_node_t> {
	typedef call_tree_base_t<ordered_node_t> Base;

public:
	call_tree_t(const actions_set_t &actions_set): Base(actions_set) {}

	/*!
	 * \brief frees memory consumed by call tree
	 */
	~call_tree_t() {}

	/*!
	 * \brief Adds new child to \a node with \a action_code
	 * \param node Target node
	 * \param action_code Child's action code
	 * \return Pointer to newly created child of \a node with \a action_code
	 */
	p_node_t add_new_link(p_node_t node, int action_code) {
		p_node_t action_node = new_node(action_code);
		nodes[node].links.push_back(std::make_pair(action_code, action_node));
		return action_node;
	}

	/*!
	 * \brief Adds new child to \a node with \a action_code if it's missing
	 * \param node Target node
	 * \param action_code Child's action code
	 * \return Pointer to child of \a node with \a action_code
	 */
	p_node_t add_new_link_if_missing(p_node_t node, int action_code) {
		return add_new_link(node, action_code);
	}

	using Base::to_json;
	using Base::merge_into;

private:
};

/*!
 * \brief Concurrent version of time stats tree to handle simultanious updates
 */
template<typename TreeType>
class concurrent_call_tree_t {
public:
	typedef TreeType unordered_call_tree_t;

	/*!
	 * \brief Initializes time_stats_tree with \a actions_set
	 * \param actions_set Set of available action for monitoring
	 */
	concurrent_call_tree_t(actions_set_t &actions_set): time_stats_tree(actions_set) {
	}

	/*!
	 * \brief Gets ownership of time stats tree
	 */
	void lock() const {
		tree_mutex.lock();
	}

	/*!
	 * \brief Releases ownership of time stats tree
	 */
	void unlock() const {
		tree_mutex.unlock();
	}

	/*!
	 * \brief Returns inner time stats tree
	 * \return Inner time stats tree
	 */
	TreeType& get_time_stats_tree() {
		return time_stats_tree;
	}

	/*!
	 * \brief Returns copy of inner time stats tree
	 * \return Copy of inner time stats tree
	 */
	TreeType copy_time_stats_tree() const {
		lock();
		TreeType time_stats_tree_copy = time_stats_tree;
		unlock();
		return time_stats_tree_copy;
	}

private:
	/*!
	 * \brief Lock to handle concurrency during updates
	 */
	mutable std::mutex tree_mutex;

	/*!
	 * \brief Inner time_stats_tree
	 */
	TreeType time_stats_tree;
};

} // namespace react

#endif // call_tree_hpp
