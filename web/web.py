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
    return tree_dict['call_tree']['react_aggregator']


def get_page():
    try:
        response = urllib2.urlopen('http://{}/{}'.format(monitored_host, 'call_tree'))
        html = response.read()
        return html
    except urllib2.URLError:
        pass

trees = {}
last_actions_trees = {}
actions_with_name = {}
min_timestamp = None
max_timestamp = None

def process_tree(tree):
    tree_id = tree['id']
    global trees
    if tree_id in trees:
        return

    trees[tree_id] = tree

    actions = []
    main_action_name = tree['actions'][0]['name']
    last_actions_trees[main_action_name] = tree
    get_actions(tree, actions, 0, True)
    for action in actions:
        action_name = action['name']
        if not action_name in actions_with_name:
            actions_with_name[action_name] = []
        del action['name']
        del action['color']
        global min_timestamp
        global max_timestamp
        if (min_timestamp == None or min_timestamp > action['startTime']):
            min_timestamp = action['startTime']

        if (max_timestamp == None or max_timestamp < action['startTime']):
            max_timestamp = action['startTime']

        actions_with_name[action_name].append(action)



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


quantiles = [
    (0.5, '50%'),
    (0.7, '75%'),
    (0.9, '90%'),
    (0.95, '95%'),
    (0.99, '99%')
]


def quintiles_measurement(previous_bucket, bucket_actions_times):
    measurement = {'timestamp': previous_bucket * 1000}
    size = len(bucket_actions_times)
    bucket_actions_times = sorted(bucket_actions_times)

    for quantile in quantiles:
        pos = int(size * quantile[0])
        value = bucket_actions_times[pos]
        measurement[quantile[1]] = value

    measurement['calls'] = size
    return measurement


def build_stacked_histogram(name, actions):
    histogram_json = []
    previous_bucket = 0
    bucket_actions_times = []

    histogram_json.append(quintiles_measurement(min_timestamp // 1000000, [0]))

    for action in actions:
        bucket = action['startTime'] // 1000000
        if (bucket != previous_bucket):
            if len(bucket_actions_times) > 0:
                histogram_json.append(quintiles_measurement(previous_bucket, bucket_actions_times))
            bucket_actions_times = []
            previous_bucket = bucket

        bucket_actions_times.append(action['endTime'] - action['startTime'])

    if len(bucket_actions_times) > 0:
        histogram_json.append(quintiles_measurement(previous_bucket, bucket_actions_times))

    histogram_json.append(quintiles_measurement(max_timestamp // 1000000, [0]))

    return render_template("stacked_histogram.html", title=name,
                           div_name="Stacked_histogram_" + name,
                           data_provider=json.dumps(histogram_json))


def render_stacked_histograms():
    rendered_histograms = []
    for name in sorted(actions_with_name.keys()):
        actions_with_name[name] = sorted(actions_with_name[name], key=lambda x: x['startTime'])
        rendered_histograms.append(build_stacked_histogram(name, actions_with_name[name]))
    return rendered_histograms

import thread
import time


def update_trees(delay):
    while True:
        try:
            for tree in get_trees(get_page()):
                process_tree(tree)
        except:
            print('Failed to download')
        time.sleep(delay)

thread.start_new_thread(update_trees, (5, ))

@app.route('/')
@app.route('/index')
def index():
    trees_content = render_template("trees_list.html", trees=[render_tree(tree) for tree in last_actions_trees.values()])

    stacked_histograms_content = render_template("stacked_histograms_list.html", histograms=render_stacked_histograms())

    return render_template("index.html", host=monitored_host,
                           trees_number=len(trees),
                           stacked_histograms_content=stacked_histograms_content,
                           trees_content=trees_content)

@app.route('/set_host', methods=['POST'])
def set_host():
    global monitored_host
    host = request.form['host']
    monitored_host = host
    return redirect('/')

if __name__ == '__main__':
    app.run()
