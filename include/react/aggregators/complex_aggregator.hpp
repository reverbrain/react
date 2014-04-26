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

#ifndef COMPLEX_AGGREGATOR_HPP
#define COMPLEX_AGGREGATOR_HPP

#include "aggregator.hpp"

namespace react {

class complex_aggregator_t : public aggregator_t {
public:
	complex_aggregator_t(const actions_set_t &actions_set): aggregator_t(actions_set) {}
	~complex_aggregator_t() {}

	void add_aggregator(std::shared_ptr<aggregator_t> aggregator) {
		aggregators.push_back(aggregator);
	}

	void aggregate(const call_tree_t &call_tree) {
		for (auto it = aggregators.begin(); it != aggregators.end(); ++it) {
			it->aggregate(call_tree);
		}
	}

	void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const {
		rapidjson::Value complex_aggregator_value(rapidjson::kArrayType);
		for (auto it = aggregators.begin(); it != aggregators.end(); ++it) {
			rapidjson::Value aggregator_value(rapidjson::kObjectType);
			it->to_json(aggregator_value, allocator);
			complex_aggregator_value.PushBack(aggregator_value, allocator);
		}
		value.AddMember("complex_aggregator", complex_aggregator_value, allocator);
	}

private:
	std::vector<std::shared_ptr<aggregator_t>> aggregators;
};

} // namespace react

#endif // COMPLEX_AGGREGATOR_HPP
