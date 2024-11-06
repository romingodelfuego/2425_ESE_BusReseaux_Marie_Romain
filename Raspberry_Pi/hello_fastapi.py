from fastapi import FastAPI,HTTPException,Request
from fastapi.responses import JSONResponse
from fastapi.templating import Jinja2Templates

app = FastAPI()
templates = Jinja2Templates(directory="templates")  # Répertoire contenant les templates HTML

welcome = "Welcome to 3ESE API!"


@app.get('/')
def hello_world():
    return 'Hello World!\n'


@app.get('/api/welcome/')
def api_welcome_get():
	return welcome
@app.post('/api/welcome/')
def api_welcome_post(data: str,request:Request):
	global welcome
	welcome = data
	return {"method" : request.method, "data":data}
@app.delete('/api/welcome/')
def api_welcome_delete(request: Request):
	global welcome
	welcome=""
	return {"method" : request.method}



@app.get('/api/welcome/{index}')
def api_welcome_index_get(index: int,request:Request):
	return {"method" : request.method,"index":index, "Result":welcome[index]}
	
@app.put('/api/welcome/{index}')
@app.patch('/api/welcome/{index}')
def api_welcome_index_modify(index: int, arg:str,request:Request):
	global welcome
	welcome = welcome[:index] + arg + welcome[index:]	
	return {"method" : request.method, "index" : index, "val": welcome[index], "data":arg,"Result":welcome}

@app.delete('/api/welcome/{index}')
def api_welcome_index_delete(index: int, request:Request):
	global welcome
	welcome = welcome[:index] + "" + welcome[1+index:]	
	return {"method" : request.method, "index" : index, "val": welcome[index],"Result":welcome}


@app.get("/{full_path:path}")
async def catch_all(request: Request, full_path: str):
    raise HTTPException(status_code=404, detail=f"Page '{full_path}' non trouvée")

@app.exception_handler(HTTPException)
def custom_404_handler(request: Request, exc: HTTPException):
    if exc.status_code == 404:
        # Si la page n'est pas trouvée, afficher un template personnalisé
        return templates.TemplateResponse("page_not_found.html", {"request": request}, status_code=404)
    else:
        return JSONResponse({"detail": exc.detail}, status_code=exc.status_code)
