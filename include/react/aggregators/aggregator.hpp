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

#ifndef AGGREGATOR_HPP
#define AGGREGATOR_HPP

#include <list>

#include "../call_tree.hpp"

namespace react {

class aggregator_t {
public:
	aggregator_t(const actions_set_t &actions_set): actions_set(actions_set) {}
	virtual ~aggregator_t() {}
	virtual void aggregate(const call_tree_t &call_tree) = 0;
	virtual void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const = 0;

protected:
	const actions_set_t &actions_set;
};

} // namespace react

#endif // AGGREGATOR_HPP
