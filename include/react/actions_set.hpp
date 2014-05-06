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

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace react {

/*!
 * \brief Represents set of actions that allows defining new actions and resolving action's names by their codes
 */
class actions_set_t {
public:
	/*!
	 * \brief Value for representing no action
	 */
	static const int NO_ACTION = -1;

	/*!
	 * \brief Initializes empty actions set
	 */
	actions_set_t() {}

	/*!
	 * \brief Frees memory consumed by actions set
	 */
	~actions_set_t() {}

	/*!
	 * \brief Defines new action if action with the same name doesn't exist
	 * \param action_name New action's name
	 * \return Newly created action's code or code of already existing action with \a action_name
	 */
	int define_new_action(const std::string& action_name) {
		auto it = std::find(actions_names.begin(), actions_names.end(), action_name);
		if (it != actions_names.end()) {
			return std::distance(actions_names.begin(), it);
		}

		int action_code = actions_names.size();
		actions_names.push_back(action_name);
		return action_code;
	}

	/*!
	 * \brief Gets action's name by its \a action_code
	 * \param action_code Action's code
	 * \return Action's name
	 */
	std::string get_action_name(int action_code) const {
		if (!code_is_valid(action_code)) {
			throw std::invalid_argument("Can't get name: action_code is invalid");
		}
		return actions_names.at(action_code);
	}

	/*!
	 * \brief Checks whether \a action_code is registred in actions_set
	 * \param action_code Action's code for checking
	 * \return True if \a action_code is registred, false otherwise
	 */
	bool code_is_valid(int action_code) const {
		if (action_code == NO_ACTION) {
			return false;
		}
		return static_cast<size_t>(action_code) < actions_names.size();
	}

private:
	/*!
	 * \brief Map between actions codes and actions names
	 */
	std::vector<std::string> actions_names;
};

} // namespace react

#endif // ACTIONS_SET_HPP
