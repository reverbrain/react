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
#include "react/aggregator.hpp"

#include "react.h"

namespace react {

/*!
 * \brief Wrapper for action_guard_t with binded updater from local context
 */
class action_guard {
public:
	/*!
	 * \brief Creates action_guard and starts action with \a action_code
	 * \param Code of started action
	 */
	explicit action_guard(int action_code);

	action_guard(const action_guard &other) = delete;

	/*!
	 * \brief Stops guarded action.
	 */
	~action_guard();

	action_guard &operator =(const action_guard &other) = delete;

	/*!
	 * \brief Allows to stop action manually
	 */
	void stop();

private:
	/*!
	 * \brief Wrapped action_guard_t
	 */
	std::unique_ptr<react::action_guard_t> m_action_guard;
};

/*!
 * \brief Returns current context actions set
 * \return Current context action set
 */
const actions_set_t &get_actions_set();

/*!
 * \internal
 *
 * \brief Adds new stat to current call tree
 * \param key Name of stat
 * \param value Value of stat
 */
void add_stat_impl(const std::string &key, const react::stat_value_t &value);

/*!
 * \brief Template wrapper for add_stat_impl. Avoids specifying value type manually.
 */
template<typename T>
void add_stat(const std::string &key, const T &value) {
	add_stat_impl(key, react::stat_value_t(value));
}

/*!
 * \brief Template specification for add_stat that handles char* correctly.
 */
void add_stat(const std::string &key, const char *value);

/*!
 * \brief Creates aggregator that can be passed to subthread in order to monitor it
 *          and merge result of monitoring with current thread context
 * \return Returns shared_ptr to newly created aggregator for subthread
 */
std::shared_ptr<aggregator_t> create_subthread_aggregator();

} // namespace react

#endif // REACT_HPP
