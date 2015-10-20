import sys

sys.path.append('/usr/local/lib/python2.7/site-packages')
import serial
import time
import cv2
import vision
import numpy as np
import sys
def nothing(*arg):
        pass

class engine:

    def __init__(self, camara, mostrar_pantalla, testing_cam):
        self.PUNTO_CALIENTE   = "<D:P>"
	self.MALEZA           = "<D:M>"
        self.AVANZA           = "<A>"
        self.ult_men          = ''
	self.tiempo_sin_men   = 4 #Tiempo en el que la raspberry no envia mensajes al arduino (si sigue viendo lo mismo)
	self.camara 	      = camara
        self.mostrar_pantalla = mostrar_pantalla
        self.testing_cam      = testing_cam
        tam_punto_caliente    = 1000
        tam_maleza            = 1500
        #self.ser 	      = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)
        self.vis              = vision.vision(tam_punto_caliente, tam_maleza, self.mostrar_pantalla)

    def control(self):
        if self.mostrar_pantalla:
            cv2.namedWindow('Real')
            cv2.namedWindow('Procesada')
        cap = cv2.VideoCapture(self.camara)

        if self.testing_cam:
            cv2.createTrackbar('H-min','Real',0,255, nothing)
            cv2.createTrackbar('S-min','Real',0,255, nothing)
            cv2.createTrackbar('V-min','Real',0,255, nothing)

            cv2.createTrackbar('H-max','Real',0,255, nothing)
            cv2.createTrackbar('S-max','Real',0,255, nothing)
            cv2.createTrackbar('V-max','Real',0,255, nothing)
	tiempo = 0
        while True:
            '''
                Aqui es donde se hace la toma de decisiones:
                    donde se consideran los tiempos de espera
                    donde se comunican los eventos al arduino
            '''
            ret, img = cap.read()
            img = img[0:600, 300:500]
            tipo = self.vis.revision(img, self.testing_cam)

            try:
                if tipo == 1:
                    if self.ult_men != self.MALEZA:
			tiempo = time.time()
                        print("Maleza detectada")
                        #self.ser.write(self.MALEZA)
                elif tipo == 2:
                    if self.ult_men != self.PUNTO_CALIENTE:
			tiempo = time.time()
                        print("Punto caliente detectado")
                        #self.ser.write(self.PUNTO_CALIENTE)

            except serial.SerialException:
	               continue

	    if self.ult_men != '' and (time.time() - tiempo) > self.tiempo_sin_men:
		self.ult_men = ''

	    #response = self.ser.readline()
	    
            if self.mostrar_pantalla:
                cv2.rectangle(img,(0,200),(200,400),(0,255,0),3)
                cv2.imshow('Real', img)
                cv2.imshow('Procesada', self.vis.output)

            ch = cv2.waitKey(50)
            if ch == 27:
                break
        cap.release()
        cv2.destroyAllWindows()

motor = engine(0, True, False)
motor.control()
