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

#ifndef REACT_H
#define REACT_H

#include <stddef.h>
#include <stdbool.h>

#ifndef Q_EXTERN_C
#  ifdef __cplusplus
#    define Q_EXTERN_C extern "C"
#  else
#    define Q_EXTERN_C extern
#  endif
#endif

/*!
 * \brief Defines new action with name \a action_name and returns it's code
 * if action with this name already exists, returns it's code
 * \param action_name Name for new action
 * \return Code of action with \a action_name
 */
Q_EXTERN_C int react_define_new_action(const char *action_name);

/*!
 * \brief Checks whether react monitoring is turned on
 * \return Returns 1 if react monitoring is on and 0 otherwise
 */
Q_EXTERN_C int react_is_active();

/*!
 * \brief Creates react_context_t and returns pointer to it
 * \param react_aggregator Aggregator that will be used to aggregate react trace
 */
Q_EXTERN_C int react_activate(void *react_aggregator);

/*!
 * \brief Cleanups react_context_t by pointer
 * \return Returns error code
 */
Q_EXTERN_C int react_deactivate();

/*!
 * \brief Starts new action with action code \a action_code in thread_local context
 * \param action_code Code of action which will be started
 * \return Returns error code
 */
Q_EXTERN_C int react_start_action(int action_code);

/*!
 * \brief Starts last in thread_local context, checks whether it's code is \a action_code
 * \param action_code Code of action which will be stopped
 * \return Returns error code
 */
Q_EXTERN_C int react_stop_action(int action_code);

/*!
 *  Functions that allow to put different stats into current react context
 */
Q_EXTERN_C int react_add_stat_bool(const char *key, bool value);
Q_EXTERN_C int react_add_stat_int(const char *key, int value);
Q_EXTERN_C int react_add_stat_double(const char *key, double value);
Q_EXTERN_C int react_add_stat_string(const char *key, const char *value);

/*!
 * \brief Submits current context to aggregator
 */
Q_EXTERN_C int react_submit_progress();

#endif // REACT_H
