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

#include "call_tree.hpp"
#include "utils.hpp"

namespace react {

class aggregator_t {
public:
	aggregator_t() {}
	virtual ~aggregator_t() {}
	virtual void aggregate(const call_tree_t &call_tree) = 0;

protected:
};

class stream_aggregator_t : public aggregator_t {
public:
	stream_aggregator_t(std::ostream &os): os(os) {}
	~stream_aggregator_t() {}
	void aggregate(const call_tree_t &call_tree) {
		os << print_json_to_string(call_tree) << std::endl;
	}

private:
	std::ostream &os;
};

} // namespace react

#endif // AGGREGATOR_HPP
