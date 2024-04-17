import serial.tools.list_ports
import numpy as np

def imprimir_puertos_com_disponibles():
    puertos = serial.tools.list_ports.comports()
    PUERTOS_DISPONIBLES = list()
    if puertos:
        print("Puertos COM disponibles:")
        for puerto in puertos:
            print(puerto.device)
            PUERTOS_DISPONIBLES.append(puerto.device)
        return PUERTOS_DISPONIBLES
    else:
        print("No se encontraron puertos COM disponibles.")
        return 0
    
