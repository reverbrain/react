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

#ifndef REACT_C_H
#define REACT_C_H

#include "stddef.h"

#ifndef Q_EXTERN_C
#  ifdef __cplusplus
#    define Q_EXTERN_C extern "C"
#  else
#    define Q_EXTERN_C extern
#  endif
#endif

Q_EXTERN_C int init_time_stats_tree(void **time_stats_tree, void *actions);
Q_EXTERN_C int cleanup_time_stats_tree(void **time_stats_tree);

Q_EXTERN_C int start_action(void *time_stats_tree, int action_code);
Q_EXTERN_C int stop_action(void *time_stats_tree, int action_code);

Q_EXTERN_C int get_time_stats(void *time_stats_tree, char **time_stats, size_t *size);

#endif // REACT_C_H
