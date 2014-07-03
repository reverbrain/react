from flask import Flask
from flask import render_template
from flask import request, redirect
from random import randint
import sys

app = Flask(__name__)

app.config.from_object('config')

monitored_host = 'localhost:20000'
monitored_file = 'react.json'
monitoring_message = ""

trees = {}
last_actions_trees = {}
actions_with_name = {}
min_timestamp = None
max_timestamp = None

def change_datasource():
    global trees
    global last_actions_trees
    global actions_with_name
    global min_timestamp
    global max_timestamp

    trees = {}
    last_actions_trees = {}
    actions_with_name = {}
    min_timestamp = None
    max_timestamp = None

def set_monitored_host(host):
    change_datasource()
    global monitored_host
    global monitoring_message
    monitored_host = host
    monitoring_message = "Monitored elliptics server: " + monitored_host


def set_monitored_file(filename):
    change_datasource()
    global monitored_file
    global monitoring_message
    monitored_file = filename
    monitoring_message = "Loading json from file: " + monitored_file

import urllib2
import collections
import json


def get_trees(html):
    tree_dict = json.loads(html, object_pairs_hook=collections.OrderedDict)
    if 'call_tree' in tree_dict:
        return tree_dict['call_tree']['react_aggregator']
    else:
        return [tree_dict]


def get_from_file():
    return open(monitored_file, 'r').read()


def get_page():
    try:
        response = urllib2.urlopen('http://{}/{}'.format(monitored_host, 'call_tree'))
        html = response.read()
        return html
    except urllib2.URLError:
        pass

trees_loader = None

if (len(sys.argv) < 2):
    print("Usage: python2 web.py (file|remote) (file_path|remote_name)")
    sys.exit(0)

monitoring_type = sys.argv[1]

if (monitoring_type == 'remote'):
    trees_loader = get_page
    set_monitored_host(sys.argv[2])
elif (monitoring_type == 'file'):
    trees_loader = get_from_file
    set_monitored_file(sys.argv[2]);
else:
    print("Unknown argument 1")
    sys.exit(1)

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

    if 'mapped_size' in tree:
        mapped_size = tree['mapped_size']
    else:
        mapped_size = 0

    return render_template("tree.html", tree_id=tree['id'], div_name='tree' + tree['id'],
                           data_provider=json.dumps(actions), tree_size=mapped_size)


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


def update_trees():
    try:
        for tree in get_trees(trees_loader()):
            process_tree(tree)
    except:
        print('Failed to download')


def update_trees_thread(delay):
    while True:
        update_trees()
        time.sleep(delay)


if (monitoring_type == "remote"):
    thread.start_new_thread(update_trees_thread, (5, ))
else:
    update_trees()

@app.route('/')
@app.route('/index')
def index():
    trees_content = render_template("trees_list.html", trees=[render_tree(tree) for tree in last_actions_trees.values()])

    stacked_histograms_content = render_template("stacked_histograms_list.html", histograms=render_stacked_histograms())

    return render_template("index.html", message=monitoring_message,
                           monitoring_type=monitoring_type,
                           filename=monitored_file,
                           host=monitored_host,
                           trees_number=len(trees),
                           stacked_histograms_content=stacked_histograms_content,
                           trees_content=trees_content)


@app.route('/set_host', methods=['POST'])
def set_host():
    global monitored_host
    host = request.form['host']
    set_monitored_host(host)
    return redirect('/')


@app.route('/set_file', methods=['POST'])
def set_file():
    global monitored_file
    filename = request.form['filename']
    set_monitored_file(filename)
    update_trees()
    return redirect('/')

if __name__ == '__main__':
    app.run()
