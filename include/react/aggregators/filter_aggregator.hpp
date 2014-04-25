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

#ifndef FILTER_AGGREGATOR_HPP
#define FILTER_AGGREGATOR_HPP

#include <memory>

#include "aggregator.hpp"

namespace react {

struct filter_t {
	filter_t() {}
	virtual ~filter_t() {}

	virtual bool operator () (const call_tree_t &call_tree) const = 0;
	virtual std::string description() const { return ""; }
};

class filter_aggregator_t : public aggregator_t {
public:
	filter_aggregator_t(const actions_set_t &actions_set,
						std::shared_ptr<filter_t> filter, aggregator_t &next_aggregator):
		aggregator_t(actions_set), filter(filter), next_aggregator(next_aggregator) {}
	~filter_aggregator_t() {}

	void aggregate(const call_tree_t &call_tree) {
		if ((*filter)(call_tree)) {
			next_aggregator.aggregate(call_tree);
		}
	}

	void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const {
		rapidjson::Value filter_aggregator_value(rapidjson::kObjectType);

		rapidjson::Value next_aggregator_value(rapidjson::kObjectType);
		next_aggregator.to_json(next_aggregator_value, allocator);
		filter_aggregator_value.AddMember("filter", filter->description().c_str(), allocator);
		filter_aggregator_value.AddMember("next_aggregator", next_aggregator_value, allocator);

		value.AddMember("filter_aggregator", filter_aggregator_value, allocator);
	}

private:
	std::shared_ptr<filter_t> filter;
	aggregator_t &next_aggregator;
};

} // namespace react

#endif // FILTER_AGGREGATOR_HPP
