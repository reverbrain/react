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

#ifndef ACTIONS_SET_HPP
#define ACTIONS_SET_HPP

namespace react {

/*!
 * \brief Represents set of actions that allows defining new actions and resolving action's names by their codes
 */
class actions_set_t {
public:
	/*!
	 * \brief Initializes empty actions set
	 */
	actions_set_t() {}

	/*!
	 * \brief Frees memory consumed by actions set
	 */
	~actions_set_t() {}

	/*!
	 * \brief Defines new action
	 * \param action_name new action's name
	 * \return new action's code
	 */
	int define_new_action(const std::string& action_name) {
		int action_code = actions_names.size();
		actions_names.push_back(action_name);
		return action_code;
	}

	/*!
	 * \brief Gets action's name by it's \a action_code
	 * \param action_code
	 * \return action's name
	 */
	std::string get_action_name(int action_code) const {
		return actions_names.at(action_code);
	}

private:
	/*!
	 * \brief Map between action's codes and action's names
	 */
	std::vector<std::string> actions_names;
};

} // namespace react

#endif // ACTIONS_SET_HPP
