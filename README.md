# React [![Build Status](https://travis-ci.org/reverbrain/react.png?branch=master)](https://travis-ci.org/reverbrain/react)
#### Realtime call tree for C++

React is a library for measuring time consumption of different parts of your program. 
You can think of it as a real-time callgrind with no overhead and user-defined call branches.

Some design solutions were inspired by [Dapper](http://research.google.com/pubs/pub36356.html) but unlike it, 
the lowest degree of granularity in React is **call tree**, which is more appropriate for low level high-performance systems written in C++ or C.

### Examples
```cpp

/* Define set of actions that will be monitored */
const int ACTION_READ = react_define_new_action("READ");
const int ACTION_FIND = react_define_new_action("FIND");
const int ACTION_LOAD_FROM_DISK = react_define_new_action("LOAD FROM DISK");

string cache_read() {
  /* Creates new guard and starts ACTION_READ which will be stopped on guard's destructor */
  action_guard read_guard(ACTION_READ);

  /* Starts ACTION_FIND which will be inner to ACTION_READ */
  react_start_action(ACTION_FIND);
  bool found = find_record();
  react_stop_action(ACTION_FIND);

  if (!found) {
    action_guard load_from_disk_guard(ACTION_LOAD_FROM_DISK);

    string data = read_from_disk();
    put_into_cache(data);
    /* Here all action guards are destructed and actions are correctly finished */
    return data; 
  }
  string data = load_from_cache();
  /* Here read_guard will be destructed and ACTION_READ will be stopped */
  return data;
}
```

[Full example](https://github.com/reverbrain/react/blob/master/examples/cpp/high_level.cpp)

Output:
```
{
    "id": "271c32e9c21d156eb9f1bea57f6ae4f1b1de3b7fd9cee2d9cca7b4c242d26c31",
    "complete": true,
    "actions": [
        {
            "name": "READ",
            "start_time": 0,
            "stop_time": 1241,
            "actions": [
                {
                    "name": "FIND",
                    "start_time": 3,
                    "stop_time": 68
                },
                {
                    "name": "LOAD FROM DISK",
                    "start_time": 69,
                    "stop_time": 1241,
                    "actions": [
                        {
                            "name": "READ FROM DISK",
                            "start_time": 70,
                            "stop_time": 1130
                        },
                        {
                            "name": "PUT INTO CACHE",
                            "start_time": 1132,
                            "stop_time": 1240
                        }
                    ]
                }
            ]
        }
    ]
}
```
### Installation
Scripts for building **deb** and **rpm** packages are included into sources.

### Compilers
* **GCC 4.4+**
* **Clang 3.4+**

### Dependencies
* Boost
* [Celero](https://github.com/DigitalInBlue/Celero) (if you want to run benchmarks)

### Documentation
[**Official wiki**](http://doc.reverbrain.com/react:react)
