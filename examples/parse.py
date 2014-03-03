#!/usr/bin/env python

import json
import collections
import pickle
import socket
import time

CARBON_SERVER = 'localhost'
CARBON_PORT = 2003

MONITOR_SERVER = 'localhost'
MONITOR_PORT = 1027

sleepTime = 0.1

import graphitesend

def send_graphite_stat(path, value):
    print(path)
    print(value)
    g = graphitesend.init(group=path, graphite_server='localhost')
    g.send('time', value / sleepTime)

prev_values = {}
def generate_stat(path, value):
    if path in prev_values.keys():
        new_value = value - prev_values[path]
        prev_values[path] = value
    else:
        prev_values[path] = value
        return

    value = new_value

    print('{} {}'.format(path, value))
    # send_graphite_stat(path, value)

def dfs_time_stats(tree, path):
    for key, value in tree.iteritems():
        if key == 'time':
            generate_stat(path, value)
        else:
            dfs_time_stats(tree[key], path + '.' + key)


def dfs(tree):
    for key, value in tree.iteritems():
        if (type(tree[key]) is collections.OrderedDict):
            if key == 'time_stats':
                dfs_time_stats(tree[key], 'time_stats');
            else:
                dfs(tree[key])

import urllib2

while True:
    try:
        response = urllib2.urlopen('http://{}:{}/{}'.format(MONITOR_SERVER, MONITOR_PORT, 'cache'))
        html = response.read()
        tree_dict = json.loads(html, object_pairs_hook=collections.OrderedDict)
        dfs(tree_dict)
    except urllib2.URLError:
        prev_values = {}
        pass

    time.sleep(sleepTime)
