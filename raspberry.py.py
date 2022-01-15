#importação de bibliotecas 
import socket
import serial
from matplotlib.pyplot import *
from tkinter import *
from time import sleep
import numpy as np

#verifica em qual porta serial canhão está conectado
try:
    arduino = serial.Serial('COM6', 9600)
except serial.SerialException:
    arduino = serial.Serial('/dev/ttyUSB1', 9600)

#cria conexão socket
s = socket.socket()
s.bind(('0.0.0.0', 8090))
s.listen(0)

#função cria janela principal 
def criaDisplay():
    global window, pressao, txt, metros, texto, botao1

    window = Tk()
    window.geometry('280x478')
    window.title("Lançamento de projétil")
    window.configure(bg='black')
    window.overrideredirect(True)
    window.overrideredirect(False)
    #window.attributes('-zoomed', True)

    txt = StringVar()
    metros = DoubleVar()

    botao1 = Button(window, text='Iniciar lançamento', font='System 20 ', command=iniciar, bg='white', fg='black')
    botao1.pack(side='top', pady='10')
    #botao2 = Button(window, text='Velocidade', font='impact 20 ', command=botao_velocidade, bg='white',
    #                fg='black').place(x=10, y=100)
    botao_sair = Button(window, text='Sair', font='System 12 ', command=terminate, bg='white', fg='black')
    botao_sair.pack(side='bottom', pady='10')
    texto = Label(window, textvariable=txt, font='System 18', fg='white', bg='black')
    texto.pack(side='top', pady='30')

    window.mainloop()


def iniciar():
    global Scala, OK

    window.configure(bg='black')
    txt.set("Insira a distância:")
    Scala = Scale(window, from_=0, to=100, orient=HORIZONTAL, length=250, width=30, resolution=0.5, variable=metros, font='System 12')
    Scala.pack(side='top')
    OK = Button(window, text='OK', font='System 18', command=ok, bg='white', fg='black')
    OK.pack(side='top')


def ok():

    #funçao presao X distancia a fazer ex:
    pressao = metros.get()

    Scala.pack_forget()
    OK.pack_forget()
    txt.set('Atire ao sinal...')
    window.update()
    sleep(1)


    while True:
        
        arduino.write(b'1')

        window.update()
    
        if arduino.is_open:
        
            recebido = arduino.readline()
            
            if len(recebido) > 0:
            
                psi = recebido.decode("utf-8", errors='ignore')
                
                print(psi)
                
                try:
                    psi = float(psi)
                except ValueError:
                    return
                    
                txt.set(psi)
                   
                if psi in np.arange((pressao-0.1), (pressao+0.1), 0.01):

                    window.configure(bg='#00FF00')
                    txt.set('LANÇAR!')
                    arduino.write(b'3')
                    window.update()
                    sleep(1)
                    break

    
    arduino.flushInput()
    arduino.flushOutput()
    
    #VELOCIDADE
    
    arduino.write(b'2')

    while True:
        
        window.update()
        
        if arduino.is_open:
            
            recebido = arduino.readline()
                
            if len(recebido) > 0:
            
                v = recebido.decode("utf-8", errors='ignore')
                
                print("Velocidade: ", v)
                
                vs = v + " m/s"
            
                txt.set(vs)
                
                break
'''
    while True: 

        client, addr = s.accept()
    
        while True:
            
            content = client.recv(32)
            
            if len(content) == 0:
                break
            
            else:
                print(content)
                txt.set("Dados recebidos")
                
        print("Fechando conexao")
        client.close()
'''


def terminate():
    # Acao do botao SAIR
    global window
    window.destroy()
    arduino.close()


criaDisplay()
