from flask import Flask
from flask import render_template
from flask import request, redirect
from random import randint

app = Flask(__name__)

app.config.from_object('config')

monitored_host = 'localhost:20000'

import urllib2
import collections
import json


def get_trees(html):
    tree_dict = json.loads(html, object_pairs_hook=collections.OrderedDict)
    return tree_dict['call_tree']['react_aggregator']['category_filter_aggregator']['categories'][0]['complex_aggregator'][0]['recent_trees_aggregator']


def get_page():
    try:
        response = urllib2.urlopen('http://{}/{}'.format(monitored_host, 'call_tree'))
        html = response.read()
        return html
    except urllib2.URLError:
        pass

trees = {}
actions_times = {}
actions_times_snapshots = []


def process_tree(tree, timestamp_actions_times):
    tree_id = tree['id']
    global trees
    if tree_id in trees:
        return

    trees[tree_id] = tree

    actions = []
    delta = tree['actions'][0]['start_time']
    get_actions(tree, actions, delta, True)
    for action in actions:
        action_name = action['name']
        delta_time = action['endTime'] - action['startTime']
        if not action_name in timestamp_actions_times:
            timestamp_actions_times[action_name] = []
        timestamp_actions_times[action_name].append(delta_time)


def get_actions(tree, actions, delta, root):
    if not root:
        actions.append({"name": tree['name'],
                        "startTime": tree['start_time'] - delta,
                        "endTime": tree['stop_time'] - delta,
                        "color": "#%06x" % randint(0, 0xFFFFFF)})

    if 'actions' in tree:
        for action in tree['actions']:
            get_actions(action, actions, delta, False)


def render_tree(tree):
    actions = []
    delta = tree['actions'][0]['start_time']
    get_actions(tree, actions, delta, True)

    return render_template("tree.html", tree_id=tree['id'], div_name='tree' + tree['id'],
                           data_provider=json.dumps(actions))


def build_histogram(key, value):
    from numpy import histogram
    hist, bin_edges = histogram(value)

    histogram_json = []
    for (value, bin_edge) in zip(hist, bin_edges):
        histogram_json.append({"time": str(bin_edge), "samples": value})
    print(histogram_json)

    return render_template("histogram.html", div_name=key, data_provider=json.dumps(histogram_json))


def render_histograms():
    rendered_histograms = []
    global actions_times
    for (key, value) in actions_times.iteritems():
        rendered_histograms.append(build_histogram(key, value))

    return rendered_histograms

import thread
import time


def update_trees(delay):
    while True:
        try:
            timestamp_actions_times = {}
            for tree in get_trees(get_page()):
                process_tree(tree, timestamp_actions_times)

            global actions_times
            for (key, value) in timestamp_actions_times.iteritems():
                if not key in actions_times:
                    actions_times[key] = []
                actions_times[key] += value
            actions_times_snapshots.append(timestamp_actions_times)
        except:
            print('Failed to download')
        time.sleep(delay)

thread.start_new_thread(update_trees, (1, ))

@app.route('/')
@app.route('/index')
def index():
    trees_content = render_template("trees_list.html", trees=[render_tree(tree) for tree in trees.values()[-10:]])

    histograms_content = render_template("histograms_list.html", histograms=render_histograms())

    return render_template("index.html", host=monitored_host,
                           trees_number=len(trees),
                           histograms_content=histograms_content,
                           trees_content=trees_content)

@app.route('/set_host', methods=['POST'])
def set_host():
    global monitored_host
    host = request.form['host']
    monitored_host = host
    return redirect('/')

if __name__ == '__main__':
    app.run()
