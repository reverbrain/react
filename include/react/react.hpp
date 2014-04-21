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

#ifndef REACT_HPP
#define REACT_HPP

#include <memory>

#include "react/call_tree.hpp"
#include "react/updater.hpp"
#include "react/aggregators/aggregator.hpp"

#include "react.h"

namespace react {

class action_guard {
public:
	explicit action_guard(int action_code);
	action_guard(const action_guard &other) = delete;
	~action_guard();

	action_guard &operator =(const action_guard &other) = delete;

	void stop();

private:
	std::unique_ptr<react::action_guard_t> m_action_guard;
};

const actions_set_t &get_actions_set();

call_tree_t get_react_context_call_tree(react_context_t *react_context);

} // namespace react

#endif // REACT_HPP
