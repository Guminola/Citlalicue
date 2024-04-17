##################################
## GUI representación de datos  ##
## COHETERÍA UDG SPACE          ##
## EQUIPO DIVEC                 ##
## MISION: CITLALLICUE          ##
##################################

#Librerías entorno de ventanas
from tkinter import Tk, Frame,Button,Label, ttk, messagebox
import tkinter as tk

#librerias comunicacion
import serial

#Librerías para graficar
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.backend_bases import key_press_handler
from matplotlib.backends.backend_tkagg import NavigationToolbar2Tk
from matplotlib.figure import Figure
from drawnow import drawnow

#Librerías de manejo de vectores
import numpy as np

#librería imagenes
from PIL import Image, ImageTk


#Librerías privadas
import PORT_Identif

#IMU
x_data = []
y_data = []
z_data = []
xacc_data = []
yacc_data = []
zacc_data = []

#BMP
altitud_data = []
presion_data = []
temp_data = []

#GPS
lat_data = []
alt_data = []
long_data = []

#CONSUMO
Corriente_data = []

#Humedad
Humedad_data = []

# Función para inicializar la comunicación serial
def iniciar_comunicacion(puerto, baudios):
    global ser
    print(puerto, baudios)
    ser = serial.Serial(puerto, baudios)
    dato = ser.readline().decode('utf-8').strip()
    print(dato)
    #recibir_datos()
    #graficar_datos()

def recibir_datos():
    dato = ser.readline().decode('utf-8').strip()
    print(dato)
    # Decodificar los datos en formato hexadecimal
    header = dato[0]
    # Obtener la parte requerida del mensaje en hexadecimal
    float_data = float(dato[1:])
    match header:
        case 'P': 
            presion_data.append(float_data)
        case 'H':
            altitud_data.append(float_data)
        case 'T':
            temp_data.append(float_data)
    ventana.after(100,recibir_datos)

def graficar_datos():   
    line_altitud, = ax1.plot(x_data, altitud_data, 
            color ='b', linestyle='solid')
    line_presion, = ax2.plot(x_data, presion_data, 
            color ='r', linestyle='solid')
    if len(x_data)>100:
        ax1.autoscale()
        ax2.autoscale()
    
    # Actualizar los datos de las líneas
    line_altitud.set_ydata(altitud_data)
    line_presion.set_ydata(presion_data)

    canvas.draw()
    ventana.after(1000, graficar_datos)

# Funcion para envíar texto a la unidad terrena al dar enter en el cuadro de texto
def enviar_texto(event, texto_widget):
    texto = texto_widget.get("1.0", tk.END).strip()  # Obtener el texto del widget de texto
    lineas = texto.split("\n")  # Dividir el texto en líneas
    if len(lineas) > 1:
        texto_anterior = "\n".join(lineas[:-1])  # Obtener todas las líneas excepto la última
    ultima_linea = lineas[-1]  # Obtener la última línea
    print("Última línea enviada:", ultima_linea)  # Imprimir la última línea
    #ser.write()


def Grafica_Init(Gframe):
    global fig, ax1, ax2, canvas, label, line_altitud, line_presion
    
    # Se crea una figura donde se almacena la gráfica
    fig, (ax1,ax2) = plt.subplots(2,dpi=90, figsize=(7, 5), facecolor='skyblue')
    plt.suptitle("Telemetría", color='black', size=30, family="Calibri", fontweight="bold")

    #Primer subplot
    ax1.set_title("Altitud")
    # Se configuran los ejes
    ax1.set_xlim(0,100)
    ax1.set_ylim(0, 1000)
    ax1.set_facecolor('white')

    ax1.axhline(linewidth=1, color='black')
    ax1.axvline(linewidth=1, color='black')
    ax1.grid()

    ax1.set_ylabel("Altitud (m)", color='black')
    ax1.set_xlabel("tiempo (s)", color='black')
    ax1.tick_params(direction='out', length=6, width=2,
                   colors='black',
                   grid_color='black', grid_alpha=0.3)


    #segundo subplot
    ax2.set_title("presión")
    # Se configuran los ejes
    ax2.set_xlim(0,100)
    ax2.set_ylim(0, 2000)
    ax2.set_facecolor('white')

    ax2.axhline(linewidth=1, color='black')
    ax2.axvline(linewidth=1, color='black')
    ax2.grid()

    ax2.set_ylabel("Presión (kpi)", color='black')
    ax2.set_xlabel("tiempo (s)", color='black')
    ax2.tick_params(direction='out', length=6, width=2,
                   colors='black',
                   grid_color='black', grid_alpha=0.3)



    # Se crea el canvas para la gráfica
    canvas = FigureCanvasTkAgg(fig, master=Gframe)
    canvas.draw()
    toolbar = NavigationToolbar2Tk(canvas, Gframe, pack_toolbar=False)
    toolbar.update()
    toolbar.pack(side=tk.BOTTOM, fill='x')
    canvas.get_tk_widget().pack(fill='both',expand=1)


# Configuración de la ventana principal
ventana = Tk()
ventana.title('Visualizador de datos')
ventana.wm_title('Grafica Matplotlib con Scale')
ventana.configure(bg='skyblue')

# Se configura un frame para la gráfica y uno para los botones
# G-Grafica.    I-Logo udg  B-Botones   Data-Texto recibido y enviado.
Iframe = Frame(ventana,bg='skyblue')
Iframe.grid(column=0,row=0,ipadx=0,ipady=0, sticky='w'+'n')
Gframe = Frame(ventana,   bg='skyblue')
Gframe.grid(column=0,row=1, sticky='n'+'w'+'e'+'s')
Bframe = Frame(ventana,   bg='skyblue')
Bframe.grid(column=0,row=2, sticky='s')
Dataframe = Frame(ventana, bg='skyblue')
Dataframe.grid(column=1,row=1,rowspan=3,ipadx=0,ipady=0, sticky='n')

# Se agrega un cuadro de texto para mostrar el texto enviado
text_send_label = tk.Label(Dataframe, text="Enviar Información", bg='skyblue')
text_send_label.grid(column=0,row=0, sticky='s')
text_send = tk.Text(Dataframe, wrap="word", width=20, height=20)
text_send.grid(pady=10,padx = 50)
text_send.bind("<Return>", lambda event: enviar_texto(event, text_send))

# Se agrega un cuadro de texto para mostrar el texto de sensores
p_label = tk.Label(Dataframe, text="Presion", bg='skyblue')
p_label.grid(column=1,row=0)
p = ttk.Entry(Dataframe, state="readonly")
p.grid(column=2,row=0,padx = 30)

t_label = tk.Label(Dataframe, text="Temperatura", bg='skyblue')
t_label.grid(column=1,row=1)
t = ttk.Entry(Dataframe, state="readonly")
t.grid(column=2,row=1,padx = 30)

h_label = tk.Label(Dataframe, text="Altitud", bg='skyblue')
h_label.grid(column=1,row=2)
h = ttk.Entry(Dataframe, state="readonly")
h.grid(column=2,row=2,padx = 30)



# Se agrega imagen

img = "D:\Coheteria\COHETERIA\Citlallicue\GUI\Coheteria.png"
image = Image.open(img)
image = image.resize((100,120) , resample=3)    
imagen = ImageTk.PhotoImage(master=Iframe,image=image)
logo_coheteria = Label(Iframe,image=imagen)
logo_coheteria.pack(expand=0,padx=0,pady=0)

# Configuración de la entrada para el puerto serial
puerto_label = tk.Label(Bframe, text='Puerto serial', bg='skyblue')
puerto_label.grid(column=0,row=1)
puerto_entry = ttk.Combobox(Bframe, values=PORT_Identif.imprimir_puertos_com_disponibles())
puerto_entry.grid(column=1,row=1,padx=5,ipadx=20)

# Configuración de la velocidad bauds
baudios_label = tk.Label(Bframe, text='Baud rate',bg='skyblue')
baudios_label.grid(row=2, column=0,pady=5)
baudios_entry = ttk.Combobox(Bframe,values=["4800","9600","14400","19200","38400","57600","115200","230400"])
baudios_entry.grid(row=2, column=1,pady=5)

# Boton para inicializar comunicaciones
iniciar_button = Button(Bframe, text='Iniciar comunicación', 
                        command=lambda:iniciar_comunicacion(puerto_entry.get(),int(baudios_entry.get())))
iniciar_button.grid(row=1,column=8, rowspan=2, columnspan=5)


Grafica_Init(Gframe)

# Ajuste al tamaño de la ventana
ventana.rowconfigure(1, weight=1)
ventana.columnconfigure(0, weight=1)


ventana.mainloop()





















##END OF CODE
