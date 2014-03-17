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

namespace react {

/*!
 * \brief Class for interactive building of call tree
 *
 *  Allows you to log actions in call-tree manner.
 */
template<typename TreeType>
class call_tree_updater_t {
public:
	typedef TreeType time_stats_tree_t;
	typedef concurrent_call_tree_t<TreeType> concurrent_time_stats_tree_t;

	/*!
	 * \brief Call tree node type
	 */
	typedef typename time_stats_tree_t::node_t node_t;

	/*!
	 * \brief Pointer to call tree node type
	 */
	typedef typename time_stats_tree_t::p_node_t p_node_t;

	/*!
	 * \brief Time point type
	 */
	typedef std::chrono::time_point<std::chrono::system_clock> time_point_t;

	/*!
	 * \brief Initializes updater without target tree
	 * \param max_depth Maximum monitored depth of call stack
	 */
	call_tree_updater_t(const size_t max_depth = DEFAULT_DEPTH):
		current_node(0), time_stats_tree(NULL), depth(0), max_depth(max_depth) {
		measurements.emplace(std::chrono::system_clock::now(), +time_stats_tree_t::NO_NODE);
	}

	/*!
	 * \brief Initializes updater with target tree
	 * \param time_stats_tree Tree used to monitor updates
	 * \param max_depth Maximum monitored depth of call stack
	 */
	call_tree_updater_t(concurrent_time_stats_tree_t &time_stats_tree,
						 const size_t max_depth = DEFAULT_DEPTH): max_depth(max_depth) {
		set_time_stats_tree(time_stats_tree);
		measurements.emplace(std::chrono::system_clock::now(), +time_stats_tree_t::NO_NODE);
	}

	/*!
	 * \brief Checks if all actions were correctly finished.
	 */
	~call_tree_updater_t() {
		if (depth != 0) {
			std::cerr << "~time_stats_updater(): extra measurements:" << std::endl;
			while (!measurements.empty()) {
				std::cerr << get_current_node_action_name() << std::endl;
				pop_measurement();
			}
		}
		std::lock_guard<concurrent_time_stats_tree_t> guard(*time_stats_tree);

		while (!measurements.empty()) {
			pop_measurement();
		}
	}

	/*!
	 * \brief Sets target tree for updates
	 * \param time_stats_tree Tree used to monitor updates
	 */
	void set_time_stats_tree(concurrent_time_stats_tree_t &time_stats_tree) {
		current_node = time_stats_tree.get_time_stats_tree().root;
		this->time_stats_tree = &time_stats_tree;
		depth = 0;
	}

	/*!
	 * \brief Checks whether tree for updates is set
	 * \return whether updater target tree was set
	 */
	bool has_time_stats_tree() const {
		return (time_stats_tree != NULL);
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
		++depth;
		if (get_depth() > max_depth) {
			return;
		}

		time_stats_tree->lock();
		p_node_t next_node = time_stats_tree->get_time_stats_tree().add_new_link_if_missing(current_node, action_code);
		time_stats_tree->unlock();

		measurements.emplace(start_time, current_node);
		current_node = next_node;
	}

	/*!
	 * \brief Stops last action. Updates total consumed time in call-tree.
	 * \param action_code Code of finished action
	 */
	void stop(const int action_code) {
		if (get_depth() > max_depth) {
			--depth;
			return;
		}

		std::lock_guard<concurrent_time_stats_tree_t> guard(*time_stats_tree);

		int expected_code = time_stats_tree->get_time_stats_tree().get_node_action_code(current_node);
		if (expected_code != action_code) {
			std::string expected_action_name = get_action_name(expected_code);
			std::string found_action_name = get_action_name(action_code);
			throw std::logic_error("Stopping wrong action. Expected: " + expected_action_name + ", Found: " + found_action_name);
		}
		pop_measurement();
	}

	/*!
	 * \brief Sets max monitored call stack depth to \a max_depth
	 * \param max_depth Max monitored call stack depth
	 */
	void set_max_depth(const size_t max_depth) {
		if (depth != 0) {
			throw std::logic_error("can't change max_depth during update");
		}

		this->max_depth = max_depth;
	}

	/*!
	 * \brief Gets current call stack depth
	 * \return Current call stack depth
	 */
	size_t get_depth() const {
		return depth;
	}

	/*!
	 * \brief Returns name of action with \a action_code
	 * \return Name of action with \a action_code
	 */
	std::string get_action_name(int action_code) const {
		return time_stats_tree->get_time_stats_tree().get_actions_set().get_action_name(action_code);
	}

	/*!
	 * \brief Returns name of action in current node
	 * \return Current's node action name
	 */
	std::string get_current_node_action_name() const {
		return get_action_name(time_stats_tree->get_time_stats_tree().get_node_action_code(current_node));
	}

private:
	/*!
	 * \internal
	 *
	 * \brief Returns delta between two time_points
	 */
	template<class Period = std::chrono::microseconds>
	long long int delta(time_point_t& start, const time_point_t& end) const {
		return (std::chrono::duration_cast<Period> (end - start)).count();
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
	 * \param end_time End time of the measurement
	 */
	void pop_measurement(const time_point_t& end_time = std::chrono::system_clock::now()) {
		measurement previous_measurement = measurements.top();
		measurements.pop();
		time_stats_tree->get_time_stats_tree().inc_node_time(current_node, delta(previous_measurement.start_time, end_time));
		time_stats_tree->get_time_stats_tree().inc_node_calls_number(current_node);
		current_node = previous_measurement.previous_node;
		--depth;
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
	concurrent_time_stats_tree_t* time_stats_tree;

	/*!
	 * \brief Current call stack depth
	 */
	size_t depth;

	/*!
	 * \brief Maximum monitored call stack depth
	 */
	size_t max_depth;

	/*!
	 * \brief Default monitored call stack depth
	 */
	static const size_t DEFAULT_DEPTH = -1;
};

/*!
 * \brief Auxiliary class for logging actions with variable place of stop time (branching/end of function/end of scope)
 */
template<typename TreeType>
class action_guard {
public:
	/*!
	 * \brief Initializes guard and starts action with \a action_code
	 * \param updater Updater whos start is called
	 * \param action_code Code of new action
	 */
	action_guard(call_tree_updater_t<TreeType> &updater, const int action_code):
		updater(updater), action_code(action_code), is_stopped(false) {
		updater.start(action_code);
	}

	/*!
	 * \brief Stops action if it is not already stopped
	 */
	~action_guard() {
		if (!is_stopped) {
			updater.stop(action_code);
		}
	}

	/*!
	 * \brief Allows to stop action manually
	 */
	void stop() {
		if (is_stopped) {
			throw std::logic_error("action " + updater.get_action_name(action_code) + " is already stopped");
		}

		updater.stop(action_code);
		is_stopped = true;
	}

private:
	/*!
	 * \brief Updater whos start/stop are called
	 */
	call_tree_updater_t<TreeType> &updater;

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
