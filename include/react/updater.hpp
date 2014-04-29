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

#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <stack>
#include <stdexcept>

#include "call_tree.hpp"

namespace react {

/*!
 * \brief Class for interactive building of call tree
 *
 *  Allows you to log actions in call-tree manner.
 */
class call_tree_updater_t {
public:
	/*!
	 * \brief Call tree node type
	 */
	typedef call_tree_t::node_t node_t;

	/*!
	 * \brief Pointer to call tree node type
	 */
	typedef call_tree_t::p_node_t p_node_t;

	/*!
	 * \brief Time point type
	 */
	typedef std::chrono::time_point<std::chrono::system_clock> time_point_t;

	/*!
	 * \brief Default monitored call stack depth
	 */
	static const size_t DEFAULT_MAX_TRACE_DEPTH = -1;

	/*!
	 * \brief Initializes updater without target tree
	 * \param max_depth Maximum monitored depth of call stack
	 */
	call_tree_updater_t(const size_t max_depth = DEFAULT_MAX_TRACE_DEPTH):
		current_node(+call_tree_t::NO_NODE), call_tree(NULL),
		trace_depth(0), max_trace_depth(max_depth) {
		measurements.emplace(std::chrono::system_clock::now(), +call_tree_t::NO_NODE);
	}

	/*!
	 * \brief Initializes updater with target tree
	 * \param call_tree Tree used to monitor updates
	 * \param max_depth Maximum monitored depth of call stack
	 */
	call_tree_updater_t(concurrent_call_tree_t &call_tree,
			const size_t max_depth = DEFAULT_MAX_TRACE_DEPTH):
		current_node(+call_tree_t::NO_NODE), call_tree(NULL),
		trace_depth(0), max_trace_depth(max_depth) {
		set_call_tree(call_tree);
		measurements.emplace(std::chrono::system_clock::now(), +call_tree_t::NO_NODE);
	}

	/*!
	 * \brief Checks if all actions were correctly finished.
	 */
	~call_tree_updater_t() {
		try {
			check_for_extra_measurements();
		} catch (std::logic_error &e) {
			std::cerr << e.what() << std::endl;
		}
	}

	/*!
	 * \brief Sets target tree for updates
	 * \param call_tree Tree used to monitor updates
	 */
	void set_call_tree(concurrent_call_tree_t &call_tree) {
		check_for_extra_measurements();
		current_node = call_tree.get_call_tree().root;
		this->call_tree = &call_tree;
		trace_depth = 0;
	}

	/*!
	 * \brief Resets current call tree
	 */
	void reset_call_tree() {
		check_for_extra_measurements();
		current_node = call_tree_t::NO_NODE;
		this->call_tree = NULL;
		trace_depth = 0;
	}

	/*!
	 * \brief Checks whether tree for updates is set
	 * \return whether updater target tree was set
	 */
	bool has_call_tree() const {
		return (call_tree != NULL);
	}

	/*!
	 * \brief Starts new branch in tree with action \a action_code
	 * \param action_code Code of new action
	 */
	void start(const int action_code) {
		start(action_code, std::chrono::system_clock::now());
	}

	/*!
	 * \brief Starts new branch in tree with action \a action_code and with specified start time
	 * \param action_code Code of new action
	 * \param start_time Action start time
	 */
	void start(const int action_code, const time_point_t& start_time) {
		if (!action_code_is_valid(action_code)) {
			throw std::invalid_argument(
						"Can't start action: action code is invalid: "
						+ std::to_string(static_cast<long long>(action_code))
			);
		}

		++trace_depth;
		if (get_trace_depth() > max_trace_depth) {
			return;
		}

		p_node_t next_node = call_tree_t::NO_NODE;
		{
			std::lock_guard<concurrent_call_tree_t> guard(*call_tree);
			next_node = call_tree->get_call_tree().add_new_link(current_node, action_code);
		}

		measurements.emplace(start_time, current_node);
		current_node = next_node;
	}

	/*!
	 * \brief Stops last action. Updates total consumed time in call-tree.
	 * \param action_code Code of finished action
	 */
	void stop(const int action_code) {
		if (!action_code_is_valid(action_code)) {
			throw std::invalid_argument(
						"Can't stop action: action code is invalid: "
						+ std::to_string(static_cast<long long>(action_code))
			);
		}

		if (get_trace_depth() > max_trace_depth) {
			--trace_depth;
			return;
		}

		std::lock_guard<concurrent_call_tree_t> guard(*call_tree);

		int expected_code = call_tree->get_call_tree().get_node_action_code(current_node);
		if (expected_code != action_code) {
			std::string expected_action_name = get_action_name(expected_code);
			std::string found_action_name = get_action_name(action_code);
			throw std::logic_error("Stopping wrong action. Expected: " + expected_action_name + ", Found: " + found_action_name);
		}
		pop_measurement();
	}

	/*!
	 * \brief Gets max allowed call stack depth
	 * \return Max allowed call stack depth
	 */
	size_t get_max_trace_depth() const {
		return max_trace_depth;
	}

	/*!
	 * \brief Sets max monitored call stack depth to \a max_depth
	 * \param max_depth Max monitored call stack depth
	 */
	void set_max_trace_depth(const size_t max_depth) {
		if (trace_depth != 0) {
			throw std::logic_error("can't change max_depth during update");
		}

		this->max_trace_depth = max_depth;
	}

	/*!
	 * \brief Gets current call stack depth
	 * \return Current call stack depth
	 */
	size_t get_actual_trace_depth() const {
		return measurements.size() - 1;
	}

	/*!
	 * \brief Gets current intended call stack depth
	 * \return Current intended call stack depth
	 */
	size_t get_trace_depth() const {
		return trace_depth;
	}

	/*!
	 * \brief Returns name of action with \a action_code
	 * \return Name of action with \a action_code
	 */
	std::string get_action_name(int action_code) const {
		if (!has_call_tree()) {
			throw std::logic_error("Can't get action name: tree is not set");
		}

		return call_tree->get_call_tree().get_actions_set().get_action_name(action_code);
	}

	/*!
	 * \brief Checks whether \a action_code is registred in actions_set
	 * \return True if \a action_code is registred, false otherwise
	 */
	bool action_code_is_valid(int action_code) {
		if (!has_call_tree()) {
			throw std::logic_error("Can't check action_code validity: tree is not set");
		}

		return call_tree->get_call_tree().get_actions_set().code_is_valid(action_code);
	}

	/*!
	 * \brief Returns updater's current node or NO_NODE if call tree is not set
	 * \return Pointer to updater's current node
	 */
	p_node_t get_current_node() const {
		return current_node;
	}

	/*!
	 * \brief Returns name of action in current node
	 * \return Current's node action name
	 */
	std::string get_current_node_action_name() const {
		if (!has_call_tree()) {
			throw std::logic_error("Can't get action name: tree is not set");
		}

		return get_action_name(call_tree->get_call_tree().get_node_action_code(current_node));
	}

private:
	/*!
	 * \internal
	 *
	 * \brief Returns delta between two time_points
	 */
	template<class Period = std::chrono::microseconds>
	int64_t delta(const time_point_t& start, const time_point_t& end) const {
		return (std::chrono::duration_cast<Period> (end - start)).count();
	}

	/*!
	 * \internal
	 *
	 * \brief Assures that updater is empty
	 */
	void check_for_extra_measurements() {
		if (get_trace_depth() != 0) {
			std::string error_message;
			if (!call_tree) {
				error_message = "~time_stats_updater(): extra measurements, tree is NULL\n";
			} else {
				error_message = "~time_stats_updater(): extra measurements:\n";
				if (get_actual_trace_depth() != get_trace_depth()) {
					error_message +=
							std::to_string(static_cast<long long>(get_trace_depth() - get_actual_trace_depth()))
							+ " untracked actions due to max depth\n";
				}
				while (get_actual_trace_depth() > 0) {
					error_message += get_current_node_action_name() + '\n';
					pop_measurement();
				}
			}
			throw std::logic_error(error_message);
		}
	}

	/*!
	 * \brief Represents single call measurement
	 */
	struct measurement {
		/*!
		 * \brief Initializes measurement with specified start time and pointer to previous node in call stack
		 * \param time Start time
		 * \param previous_node Pointer to previous node in call stack
		 */
		measurement(const time_point_t& time, p_node_t previous_node): start_time(time),
			previous_node(previous_node) {}

		/*!
		 * \brief Start time of the measurement
		 */
		time_point_t start_time;

		/*!
		 * \brief Pointer to previous node in call stack
		 */
		p_node_t previous_node;
	};

	/*!
	 * \brief Removes measurement from top of call stack and updates corresponding node in call-tree
	 * \param stop_time End time of the measurement
	 */
	void pop_measurement(const time_point_t& stop_time = std::chrono::system_clock::now()) {
		measurement previous_measurement = measurements.top();
		measurements.pop();
		call_tree->get_call_tree().set_node_start_time(current_node, delta(time_point_t(), previous_measurement.start_time));
		call_tree->get_call_tree().set_node_stop_time(current_node, delta(time_point_t(), stop_time));
		current_node = previous_measurement.previous_node;
		--trace_depth;
	}

	/*!
	 * \brief Current position in call-tree
	 */
	p_node_t current_node;

	/*!
	 * \brief Call stack
	 */
	std::stack<measurement> measurements;

	/*!
	 * \brief Target call-tree
	 */
	concurrent_call_tree_t* call_tree;

	/*!
	 * \brief Current call stack depth
	 */
	size_t trace_depth;

	/*!
	 * \brief Maximum monitored call stack depth
	 */
	size_t max_trace_depth;
};

/*!
 * \brief Auxiliary class for logging actions with variable place of stop time (branching/end of function/end of scope)
 */
class action_guard_t {
public:
	/*!
	 * \brief Initializes guard and starts action with \a action_code
	 * \param updater Updater whos start is called
	 * \param action_code Code of new action
	 */
	action_guard_t(call_tree_updater_t *updater, const int action_code):
		updater(updater), action_code(action_code), is_stopped(false) {
		if (updater) {
			updater->start(action_code);
		}
	}

	/*!
	 * \brief Stops action if it is not already stopped
	 */
	~action_guard_t() {
		if (!is_stopped && updater) {
			updater->stop(action_code);
		}
	}

	/*!
	 * \brief Allows to stop action manually
	 */
	void stop() {
		if (is_stopped) {
			std::string error_message;

			if (updater) {
				error_message = "action "
						+ updater->get_action_name(action_code)
						+ " is already stopped";
			} else {
				error_message = "action "
						+ std::to_string(static_cast<long long>(action_code))
						+ " is already stopped";
			}

			throw std::logic_error(error_message);
		}

		if (updater) {
			updater->stop(action_code);
		}
		is_stopped = true;
	}

private:
	/*!
	 * \brief Updater whos start/stop are called
	 */
	call_tree_updater_t *updater;

	/*!
	 * \brief Action code of guarded action
	 */
	const int action_code;

	/*!
	 * \brief Shows if action is already stopped
	 */
	bool is_stopped;
};

} // namespace react

#endif // UPDATER_HPP
