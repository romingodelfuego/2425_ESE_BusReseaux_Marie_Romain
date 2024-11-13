from fastapi import FastAPI,HTTPException,Request
from fastapi.responses import JSONResponse,RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import serial 
from unittest.mock import patch
#**************INITIALISATION*************#
app = FastAPI()
templates = Jinja2Templates(directory="templates")  # Répertoire contenant les templates HTML
app.mount("/static", StaticFiles(directory="static"), name="static")

#ser = serial.Serial('/dev/ttyAMA0', 115200, timeout=1) 
#ser = serial.Serial('/dev/tty.usbmodem11303', 115200, timeout=1)

# Simulation de serial.Serial avec unittest.mock.patch 
with patch('serial.Serial') as mock_serial:
   ser = mock_serial.return_value # Configurer mock_serial pour qu'il retourne un objet simulé
#**************VARIABLES*************#
pres = [12,13,23,234]
temp = [27,28,29,23,24]
time = [1,2,3,4,5]
K = [1,2,3,4]
A = 127
K_split_character = ','
#**************FONCTIONS*************#
def uart_transmit(msg:str,val:int=None):
    msgTX= msg+"\r" if val==None else msg+"="+str(val)+"\r"
    ser.write(msgTX.encode())
    return msgTX

def uart_receive():
    msgRX=ser.readline().decode('utf-8').strip() # Fonction bloquante ?
    return msgRX
# def update_data():
#     # Ajouter une nouvelle donnée (valeur aléatoire)
#     global pres, time
#     pres.append(random.randint(0, 100))
#     time.append(len(time)+1)
#**************REQUETES SERVEUR*************#
@app.get('/')
def home(request: Request): 
    return RedirectResponse(url="/docs")
    return templates.TemplateResponse("index.html", {"request": request})


@app.get('/temp')
def retrieve_all_previous_temperature(request: Request):
	return {"temperature": temp,"method": request.method}
@app.post('/temp')
def retrieve_new_temperature(request: Request): 
    msgTX=uart_transmit("GET_T")
    msgRX =uart_receive()
    temp.append(int(msgRX))
    return {"temperature": temp[-1],"method": request.method,"UART_TX":msgTX,"UART_RX":msgRX}
@app.get('/temp/{index}')
def return_temperature_x(index:  int,request: Request): 
	return {"temperature": temp[index],"method": request.method,"index": index}
@app.delete('/temp/{index}')
def delete_temperature_x(index:  int,request:  Request): 
	temp.pop(index)
	return {"temperature": temp,"method": request.method,"index": index}


@app.get('/pres')
def return_all_previous_presure(request: Request): 
	return {"pressure": pres,"method": request.method}
@app.post('/pres')
def retrieve_new_pressure(request: Request): 
    msgTX=uart_transmit("GET_P")
    msgRX =uart_receive()
    pres.append(int(msgRX))
    return {"pressure": pres[-1],"method": request.method,"UART_TX":msgTX,"UART_RX":msgRX}
@app.get('/pres/{index}')
def return_presssure_x(index:  int,request: Request): 
	return {"pressure": pres[index],"method": request.method,"index": index}
@app.delete('/pres/{index}')
def delete_presssure_x(index:  int,request: Request): 
	pres.pop(index)
	return {"pressure": pres,"method": request.method,"index": index}


@app.post('/scale') # METHODE POST PLUTOT ?
def return_scale_K(request: Request): 
    msgTX=uart_transmit("GET_K")
    msgRX =uart_receive()
    K = msgRX.split(K_split_character)
    return {"K": K,"method": request.method,"UART_TX":msgTX,"UART_RX":msgRX}
@app.post('/scale/{index}')
def change_scale_K_for_x(index: int,value:int,request: Request):
    msgTX=uart_transmit("SET_K",value)
    msgRX =uart_receive()
    K[index] = int(msgRX)
    return {"K": K[index],"method": request.method,"index": index,"UART_TX":msgTX,"UART_RX":msgRX}


@app.get('/angle') # METHODE POST PLUTOT ?
def return_angle(request: Request): 
    msgTX=uart_transmit("GET_A")
    msgRX =uart_receive()
    A  = int(msgRX)
    return {"A": A,"method": request.method,"UART_TX":msgTX,"UART_RX":msgRX}





@app.get("/{full_path: path}")
async def catch_all_url(request:  Request, full_path:  str): 
    raise HTTPException(status_code=404, detail=f"Page '{full_path}' non trouvée")

@app.exception_handler(HTTPException)
def custom_404_handler(request:  Request, exc:  HTTPException): 
    if exc.status_code == 404: 
        return templates.TemplateResponse("page_not_found.html", {"request":  request}, status_code=404)
    else: 
        return JSONResponse({"detail":  exc.detail}, status_code=exc.status_code)