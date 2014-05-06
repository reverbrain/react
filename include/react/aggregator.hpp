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

/*!
 * \brief Aggregators base class. Represents call tree collector.
 */
class aggregator_t {
public:
	/*!
	 * \brief Constructs empty aggregator
	 */
	aggregator_t() {}

	/*!
	 * \brief Frees memory consumed by aggregator
	 */
	virtual ~aggregator_t() {}

	/*!
	 * \brief Aggregates passed call tree
	 * \param Call tree for aggregation
	 */
	virtual void aggregate(const call_tree_t &call_tree) = 0;
};

/*!
 * \brief Aggregator that outputs aggregated trees to stream
 */
class stream_aggregator_t : public aggregator_t {
public:
	/*!
	 * \brief Constructs aggregator
	 * \param os Stream where aggregated trees will be outputed
	 */
	stream_aggregator_t(std::ostream &os): os(os) {}

	/*!
	 * \brief Frees memory consumed by stream_aggregator
	 */
	~stream_aggregator_t() {}

	/*!
	 * \brief Outputs call tree into stream
	 * \param call_tree Tree that will be outputed
	 */
	void aggregate(const call_tree_t &call_tree) {
		os << print_json_to_string(call_tree) << std::endl;
	}

private:
	/*!
	 * \brief Target stream where aggregated trees will be outputed
	 */
	std::ostream &os;
};

} // namespace react

#endif // AGGREGATOR_HPP
