from threading import Thread
import time
import sys
import serial
sys.path.append('/usr/local/lib/python2.7/site-packages')
import cv2
import numpy as np

def nothing(*arg):
        pass

camara                 = 0
cap                    = cv2.VideoCapture(camara)
ret, img               = cap.read()
terminated             = False
tam_punto_caliente     = 1000
tam_maleza             = 1500
tam_defecto            = 1000
mostrar_pantalla       = True
H                      = 0;
S                      = 1;
V                      = 2;
min                    = 0;
max                    = 1;
maleza                 = [[0 for x in range(2)] for x in range(3)]
punto_caliente         = [[0 for x in range(2)] for x in range(3)]
#Datos de maleza
maleza[H][min]         = 50;
maleza[H][max]         = 100;
maleza[S][min]         = 120;
maleza[S][max]         = 255;
maleza[V][min]         = 50;
maleza[V][max]         = 255;
#Datos de punto caliente
punto_caliente[H][min] = 0;
punto_caliente[H][max] = 180;
punto_caliente[S][min] = 0;
punto_caliente[S][max] = 60;
punto_caliente[V][min] = 200;
punto_caliente[V][max] = 255;

lower_maleza           = np.array([maleza[H][min], maleza[S][min], maleza[V][min]])
upper_maleza           = np.array([maleza[H][max], maleza[S][max], maleza[V][max]])

lower_punto_caliente   = np.array([punto_caliente[H][min], punto_caliente[S][min], punto_caliente[V][min]])
upper_punto_caliente   = np.array([punto_caliente[H][max], punto_caliente[S][max], punto_caliente[V][max]])

testing                = False
tipo                   = 0
tim = 1/30
def captura():
    global img, cap, terminated
    if mostrar_pantalla:
        cv2.namedWindow('Real')
        cv2.namedWindow('Img-pro')
        cv2.namedWindow('Procesada')
    while True:
        time.sleep(tim)
        ret, img = cap.read()
        img = img[0:600, 300:500]
        if mostrar_pantalla:
            cv2.imshow('Real', img)

        ch = cv2.waitKey(100)
        if ch == 27:
            terminated = True
            break
    cap.release()
    cv2.destroyAllWindows()

def deteccion(lower, upper, tam_max_area):
    global img
    #Analisis de la imagen
    img_hsv = cv2.cvtColor(img[200:400, 0:200], cv2.COLOR_BGR2HSV)
    #Transformacion morfologica para detectar mejor
    kernel_cinco = np.ones((5,5),np.uint8);
    mascara = cv2.inRange(img_hsv, lower, upper)
    cv2.erode(mascara,kernel_cinco,iterations = 2)
    cv2.dilate(mascara,kernel_cinco,iterations = 2)
    mascara = cv2.morphologyEx(mascara, cv2.MORPH_OPEN, kernel_cinco)
    mascara = cv2.morphologyEx(mascara, cv2.MORPH_CLOSE, kernel_cinco)

    output = cv2.bitwise_and(img[200:400, 0:200], img[200:400, 0:200], mask = mascara)
    ret, thresh = cv2.threshold(mascara,127,255,0)
    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    #Busqueda del contorno mas grande, si supera un tamano establecido sera considerado punto caliente
    if len(contours) > 0:
        for index in range(len(contours)):
            area = cv2.contourArea(contours[index])
            if area >= tam_max_area:
                if mostrar_pantalla:
                    imag = img
                    cv2.rectangle(imag,(0,200),(200,400),(0,255,0),3)
                    cv2.drawContours(imag[200:400, 0:200], contours, index, (255,0,0), -1)
                    cv2.imshow('Img-pro', imag)
                    cv2.imshow('Procesada', output)
                return True
    return False

def proceso():
    global tipo
    while True:
        time.sleep(tim)
        if terminated:
            break
        if testing:
            H_min = cv2.getTrackbarPos('H-min','Real')
            S_min = cv2.getTrackbarPos('S-min','Real')
            V_min = cv2.getTrackbarPos('V-min','Real')
            lower = np.array([H_min, S_min, V_min])

            H_max = cv2.getTrackbarPos('H-max','Real')
            S_max = cv2.getTrackbarPos('S-max','Real')
            V_max = cv2.getTrackbarPos('V-max','Real')
            upper = np.array([H_max, S_max, V_max])
            if(deteccion(lower, upper, __tam_defecto, img)):
                retorno = 3
        else:
            maleza_detectada = deteccion(lower_maleza, upper_maleza, tam_maleza)
            if maleza_detectada:
                tipo = 1
                print "Maleza"
            else:
                punto_caliente_detectado = deteccion(lower_punto_caliente, upper_punto_caliente, tam_punto_caliente)
                if punto_caliente_detectado:
                    tipo = 2
                    print "Punto caliente"
                else:
                    tipo = 0

def serial_listener():
    time.sleep(tim)
    while True:
        if terminated:
            break
        if tipo == 1:
            print "Maleza"
        elif tipo == 2:
            print "Punto caliente"
        #else:
        #    print "Nada"

hilo_captura = Thread( target=captura)
hilo_proceso = Thread( target=proceso)
#hilo_serial  = Thread( target=serial_listener)


hilo_captura.start()
hilo_proceso.start()
#hilo_serial.start()
