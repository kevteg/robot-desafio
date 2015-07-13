import cv2
import vision
import numpy as np
import sys


class engine:

    def __init__(self, camara, mostrar_pantalla, testing_cam):
        self.camara = camara
        self.mostrar_pantalla = mostrar_pantalla
        self.testing_cam = testing_cam
        tam_punto_caliente = 1000
        tam_maleza = 3000
        self.vis = vision.vision(tam_punto_caliente, tam_maleza, self.mostrar_pantalla)

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
        while True:
            '''
                Aqui es donde se hace la toma de decisiones:
                    donde se consideran los tiempos de espera
                    donde se comunican los eventos al arduino
            '''
            ret, img = cap.read()
            img = img[0:600, 300:500]
            self.vis.revision(img, self.testing_cam)
            if self.mostrar_pantalla:
                cv2.rectangle(img,(0,200),(200,400),(0,255,0),3)
                cv2.imshow('Real', img)
                cv2.imshow('Procesada', self.vis.output)

            ch = cv2.waitKey(50)
            if ch == 27:
                break
        cap.release()
        cv2.destroyAllWindows()

motor = engine(1, True, False)
motor.control()
