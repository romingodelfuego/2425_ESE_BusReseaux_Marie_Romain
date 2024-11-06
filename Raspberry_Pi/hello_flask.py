import json
from flask import Flask,jsonify,render_template,abort,request
app = Flask(__name__)


@app.route('/')
def hello_world():
    return 'Hello World!\n'

welcome = "Welcome to 3ESE API!"

@app.route('/api/welcome/', methods=['GET','POST','DELETE'])
def api_welcome():
	global welcome
	if request.method=='GET':
		return welcome
	if request.method=='POST':
		welcome = request.data
	if request.method=='DELETE':
		welcome = ""
##		abort(202)
	return jsonify({"method" : request.method, "data":request.data})

@app.route('/api/welcome/<int:index>',methods=['GET','PUT','PATCH','DELETE'])
def api_welcome_index(index):
	global welcome
	if (index > len(welcome)):
		abort(404)
	if request.method == 'GET':
##		return jsonify({"method" : request.method, "args":request.args})
	if request.method == 'PATCH':
		welcome[index]=request.data
##		abort(202)
	if request.method == 'PUT':
		welcome = welcome[:index] + request.data + welcome[index:]
##		abort(202)
	if request.method =='DELETE':
		welcome[index] =""
##		abort(202)
	return jsonify({"method" : request.method,"index" : index, "val": welcome[index], "data":request.data})
@app.route('/api/request/',methods=['GET','POST'])
@app.route('/api/request/<path>',methods=['GET','POST'])
def api_request(path=None):
	resp = {
		"method": request.method,
		"url": request.url,
		"path": request.args,
		"headers": dict(request.headers),
		}
	if request.method =='POST':
		resp["POST"] = {
				"data" : request.get_json(),
				}
	return jsonify(resp)



@app.errorhandler(404)
def page_not_found(error):
	return render_template('page_not_found.html')
