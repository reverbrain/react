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

#include "stddef.h"

#ifndef Q_EXTERN_C
#  ifdef __cplusplus
#    define Q_EXTERN_C extern "C"
#  else
#    define Q_EXTERN_C extern
#  endif
#endif

// Returns action_code of newly created action
Q_EXTERN_C int react_define_new_action(const char *action_name);

struct react_call_tree_t;
struct react_call_tree_updater_t;

Q_EXTERN_C int react_is_active();

Q_EXTERN_C react_call_tree_t *react_create_call_tree();
Q_EXTERN_C int react_cleanup_call_tree(react_call_tree_t *call_tree);

Q_EXTERN_C react_call_tree_updater_t *react_create_call_tree_updater(react_call_tree_t *call_tree);
Q_EXTERN_C int react_cleanup_call_tree_updater(react_call_tree_updater_t *call_tree_updater);

Q_EXTERN_C int react_start_action(int action_code);
Q_EXTERN_C int react_stop_action(int action_code);

#endif // REACT_H
