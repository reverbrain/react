from flask import Flask
from flask import render_template

app = Flask(__name__)

app.config.from_object('config')

@app.route('/')
@app.route('/index')
def index():
    return render_template("index.html", host='"localhost:20000"')


if __name__ == '__main__':
    app.run()
