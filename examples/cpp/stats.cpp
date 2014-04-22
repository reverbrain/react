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

#include "react/react.hpp"
#include "react/aggregators/recent_trees_aggregator.hpp"
#include "react/utils.hpp"

int action_code = react_define_new_action("ACTION");

int main() {
	react::recent_trees_aggregator_t aggregator(3);
	react_context_t *context = react_activate(&aggregator);

	react::add_stat("CanPut", true);
	react::add_stat("Answer", 42);
	react::add_stat("Pi", 3.1415);
	react::add_stat("Name", "React");

	react_deactivate(context);
	print_json(aggregator);
	return 0;
}
