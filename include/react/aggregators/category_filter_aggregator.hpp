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

#ifndef CATEGORY_FILTER_AGGREGATOR_HPP
#define CATEGORY_FILTER_AGGREGATOR_HPP

#include "aggregator.hpp"

namespace react {

template<typename Category>
struct category_extractor_t {
	category_extractor_t() {}
	virtual ~category_extractor_t() {}

	virtual Category operator () (const call_tree_t &call_tree) const = 0;
};


template<typename Category>
struct stat_extractor_t : public category_extractor_t<Category> {
	stat_extractor_t(const std::string &stat_name): stat_name(stat_name) {}
	~stat_extractor_t() {}

	Category operator () (const call_tree_t &call_tree) const {
		return call_tree.get_stat<Category>(stat_name);
	}

	std::string stat_name;
};

template<typename Category>
class category_filter_aggregator_t : public aggregator_t {
public:
	category_filter_aggregator_t(const actions_set_t &actions_set,
								 std::shared_ptr<category_extractor_t<Category>> category_extractor):
		aggregator_t(actions_set), category_extractor(category_extractor) {}
	~category_filter_aggregator_t() {}

	void add_category_aggregator(Category category, std::shared_ptr<aggregator_t> aggregator) {
		categories_aggregators[category] = aggregator;
	}

	void aggregate(const call_tree_t &call_tree) {
		Category category = (*category_extractor)(call_tree);
		if (categories_aggregators.find(category) != categories_aggregators.end()) {
			categories_aggregators[category]->aggregate(call_tree);
		}
	}

	void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const {
		rapidjson::Value category_aggregator_value(rapidjson::kArrayType);
		for (auto it = categories_aggregators.begin(); it != categories_aggregators.end(); ++it) {
			Category category = it->first;
			rapidjson::Value aggregator_value(rapidjson::kObjectType);
			it->second->to_json(aggregator_value, allocator);
			aggregator_value.AddMember("category", category, allocator);
			category_aggregator_value.PushBack(aggregator_value, allocator);
		}
		value.AddMember("category_filter_aggregator", category_aggregator_value, allocator);
	}

private:
	std::shared_ptr<category_extractor_t<Category>> category_extractor;
	std::unordered_map<Category, std::shared_ptr<aggregator_t>> categories_aggregators;
};

} // namespace react

#endif // CATEGORY_FILTER_AGGREGATOR_HPP
