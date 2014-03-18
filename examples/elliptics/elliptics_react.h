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

#ifndef ELLIPTICS_REACT_H
#define ELLIPTICS_REACT_H

#include "stddef.h"

#ifndef Q_EXTERN_C
#  ifdef __cplusplus
#    define Q_EXTERN_C extern "C"
#  else
#    define Q_EXTERN_C extern
#  endif
#endif

Q_EXTERN_C int init_call_tree(void **call_tree);
Q_EXTERN_C int cleanup_call_tree(void *call_tree);
Q_EXTERN_C int init_empty_updater();
Q_EXTERN_C int init_updater(void *call_tree);
Q_EXTERN_C int cleanup_updater();
Q_EXTERN_C int set_current_call_tree(void *call_tree);
Q_EXTERN_C int call_tree_is_set();
Q_EXTERN_C int merge_call_tree(void *call_tree, void *elliptics_react_manager);

Q_EXTERN_C int start_action(int action_code);
Q_EXTERN_C int stop_action(int action_code);

extern const int ACTION_PROCESS_CMD_RAW;

extern const int ACTION_CACHE;
extern const int ACTION_CACHE_WRITE;
extern const int ACTION_CACHE_READ;
extern const int ACTION_CACHE_REMOVE;
extern const int ACTION_CACHE_LOOKUP;
extern const int ACTION_CACHE_LOCK;
extern const int ACTION_CACHE_FIND;
extern const int ACTION_CACHE_ADD_TO_PAGE;
extern const int ACTION_CACHE_RESIZE_PAGE;
extern const int ACTION_CACHE_SYNC_AFTER_APPEND;
extern const int ACTION_CACHE_WRITE_APPEND_ONLY;
extern const int ACTION_CACHE_WRITE_AFTER_APPEND_ONLY;
extern const int ACTION_CACHE_POPULATE_FROM_DISK;
extern const int ACTION_CACHE_CLEAR;
extern const int ACTION_CACHE_LIFECHECK;
extern const int ACTION_CACHE_CREATE_DATA;
extern const int ACTION_CACHE_CAS;
extern const int ACTION_CACHE_MODIFY;
extern const int ACTION_CACHE_DECREASE_KEY;
extern const int ACTION_CACHE_MOVE_RECORD;
extern const int ACTION_CACHE_ERASE;
extern const int ACTION_CACHE_REMOVE_LOCAL;
extern const int ACTION_CACHE_LOCAL_LOOKUP;
extern const int ACTION_CACHE_INIT;
extern const int ACTION_CACHE_LOCAL_READ;
extern const int ACTION_CACHE_PREPARE;
extern const int ACTION_CACHE_LOCAL_WRITE;
extern const int ACTION_CACHE_PREPARE_SYNC;
extern const int ACTION_CACHE_SYNC;
extern const int ACTION_CACHE_SYNC_BEFORE_OPERATION;
extern const int ACTION_CACHE_ERASE_ITERATE;
extern const int ACTION_CACHE_SYNC_ITERATE;
extern const int ACTION_CACHE_DNET_OPLOCK;
extern const int ACTION_CACHE_DESTRUCT;

extern const int ACTION_DNET_PROCESS_CMD_RAW;
extern const int ACTION_DNET_MONITOR_PROCESS_CMD;
extern const int ACTION_DNET_PROCESS_INDEXES;
extern const int ACTION_DNET_CMD_REVERSE_LOOKUP;
extern const int ACTION_DNET_CMD_JOIN_CLIENT;
extern const int ACTION_DNET_UPDATE_NOTIFY;
extern const int ACTION_DNET_NOTIFY_ADD;
extern const int ACTION_DNET_CMD_EXEC;
extern const int ACTION_DNET_CMD_STAT_COUNT;
extern const int ACTION_DNET_CMD_STATUS;
extern const int ACTION_DNET_CMD_AUTH;
extern const int ACTION_DNET_CMD_ITERATOR;
extern const int ACTION_DNET_CMD_BULK_READ;
extern const int ACTION_DNET_CMD_ROUTE_LIST;

#endif // ELLIPTICS_REACT_H
