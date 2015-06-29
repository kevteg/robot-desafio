import cv2
import numpy as np
import sys
def nothing(*arg):
        pass


class vision:

    def __init__(self, camara, mostrar_pantalla, testing, tam_punto):
        self.__camara = camara
        self.__mostrar_pantalla = mostrar_pantalla
        self.__punto_caliente = False
        self.__testing = testing
        self.H_min = 50
        self.H_max = 100
        self.S_min = 120
        self.S_max = 255
        self.V_min = 100
        self.V_max = 255
        self.__tam_punto = tam_punto
        self.lower = np.array([self.H_min, self.S_min, self.V_min])
        self.upper = np.array([self.H_max, self.S_max, self.V_max])

    def revision(self):
        if self.__mostrar_pantalla:
            cv2.namedWindow('Real')
            cv2.namedWindow('Procesada')
        cap = cv2.VideoCapture(self.__camara);
        if self.__testing:
            cv2.createTrackbar('H-min','Real',0,255, nothing)
            cv2.createTrackbar('S-min','Real',0,255, nothing)
            cv2.createTrackbar('V-min','Real',0,255, nothing)

            cv2.createTrackbar('H-max','Real',0,255, nothing)
            cv2.createTrackbar('S-max','Real',0,255, nothing)
            cv2.createTrackbar('V-max','Real',0,255, nothing)
        while True:
            ret, img = cap.read()
            #Analisis de la imagen
            img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
            #Si se esta probando con los valores de la imagen
            if self.__testing:
                H_min = cv2.getTrackbarPos('H-min','Real')
                S_min = cv2.getTrackbarPos('S-min','Real')
                V_min = cv2.getTrackbarPos('V-min','Real')
                self.lower = np.array([H_min, S_min, V_min])

                H_max = cv2.getTrackbarPos('H-max','Real')
                S_max = cv2.getTrackbarPos('S-max','Real')
                V_max = cv2.getTrackbarPos('V-max','Real')
                self.upper = np.array([H_max, S_max, V_max])
            #Transformacion morfologica para detectar mejor
            kernel_cinco = np.ones((5,5),np.uint8);
            mascara = cv2.inRange(img_hsv, self.lower, self.upper)
            cv2.erode(mascara,kernel_cinco,iterations = 2)
            cv2.dilate(mascara,kernel_cinco,iterations = 2)
            mascara = cv2.morphologyEx(mascara, cv2.MORPH_OPEN, kernel_cinco)
            mascara = cv2.morphologyEx(mascara, cv2.MORPH_CLOSE, kernel_cinco)

            output = cv2.bitwise_and(img, img, mask = mascara)
            ret, thresh = cv2.threshold(mascara,127,255,0)
            contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
            #Busqueda del contorno mas grande, si supera un tamano establecido sera considerado punto caliente
            if len(contours) > 0:
                area_ini = cv2.contourArea(contours[0])
                index = 0
                for i in range(1, len(contours)):
                    area = cv2.contourArea(contours[i])
                    if area >= area_ini:
                        area_ini = area
                        index = i
                if area_ini >= self.__tam_punto:
                    if self.__mostrar_pantalla:
                        cv2.drawContours(img, contours, index, (0,255,0), 3)
                    print("Punto caliente detectado")

            #Si esta activado mostrara la imagen
            if self.__mostrar_pantalla:
                cv2.imshow('Real', img)
                cv2.imshow('Procesada', output)

            ch = cv2.waitKey(5)
            if ch == 27:
                break
        cap.release()
        cv2.destroyAllWindows()

vis = vision(1, True, False, 300)
vis.revision()
