#ifndef AGGREGATOR_HPP
#define AGGREGATOR_HPP

#include <list>

#include "call_tree.hpp"

namespace react {

class aggregator_t {
public:
	aggregator_t() {}
	virtual ~aggregator_t() {}
	virtual void aggregate(const call_tree_t &call_tree) = 0;
	virtual void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const = 0;
};

class recent_trees_aggregator_t : public aggregator_t {
public:
	recent_trees_aggregator_t(size_t recent_list_size): recent_list_size(recent_list_size) {}
	~recent_trees_aggregator_t() {}

	void aggregate(const call_tree_t &call_tree) {
		recent_trees.push_back(call_tree);
		if (recent_trees.size() > recent_list_size) {
			recent_trees.pop_front();
		}
	}

	void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const {
		rapidjson::Value recent_trees_values(rapidjson::kArrayType);
		for (const auto &tree : recent_trees) {
			rapidjson::Value tree_value(rapidjson::kObjectType);
			tree.to_json(tree_value, allocator);
			recent_trees_values.PushBack(tree_value, allocator);
		}
		value.AddMember("recent_trees", recent_trees_values, allocator);
	}

private:
	size_t recent_list_size;
	std::list<call_tree_t> recent_trees;
};

} // namespace react

#endif // AGGREGATOR_HPP
