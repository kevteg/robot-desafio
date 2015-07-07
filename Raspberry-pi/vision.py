import cv2
import numpy as np
import sys
def nothing(*arg):
        pass


class vision:

    def __init__(self, camara, mostrar_pantalla, testing):
        self.__camara = camara
        self.__mostrar_pantalla = mostrar_pantalla
        self.__punto_caliente = False
        self.__testing = testing
        self.__tam_punto_caliente = 1000
        self.__tam_maleza = 2500
        self.tam_defecto = 1000
        self.H = 0;
        self.S = 1;
        self.V = 2;
        self.min = 0;
        self.max = 1;
        self.maleza         = [[0 for x in range(2)] for x in range(3)]
        self.punto_caliente = [[0 for x in range(2)] for x in range(3)]
        #Datos de maleza
        self.maleza[self.H][self.min] = 50;
        self.maleza[self.H][self.max] = 100;
        self.maleza[self.S][self.min] = 120;
        self.maleza[self.S][self.max] = 255;
        self.maleza[self.V][self.min] = 100;
        self.maleza[self.V][self.max] = 255;
        #Datos de punto caliente
        self.punto_caliente[self.H][self.min] = 0;
        self.punto_caliente[self.H][self.max] = 180;
        self.punto_caliente[self.S][self.min] = 0;
        self.punto_caliente[self.S][self.max] = 60;
        self.punto_caliente[self.V][self.min] = 200;
        self.punto_caliente[self.V][self.max] = 255;

        self.lower_maleza = np.array([self.maleza[self.H][self.min], self.maleza[self.S][self.min], self.maleza[self.V][self.min]])
        self.upper_maleza = np.array([self.maleza[self.H][self.max], self.maleza[self.S][self.max], self.maleza[self.V][self.max]])

        self.lower_punto_caliente = np.array([self.punto_caliente[self.H][self.min], self.punto_caliente[self.S][self.min], self.punto_caliente[self.V][self.min]])
        self.upper_punto_caliente = np.array([self.punto_caliente[self.H][self.max], self.punto_caliente[self.S][self.max], self.punto_caliente[self.V][self.max]])

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
            img = img[0:600, 300:500]
            if self.__testing:
                H_min = cv2.getTrackbarPos('H-min','Real')
                S_min = cv2.getTrackbarPos('S-min','Real')
                V_min = cv2.getTrackbarPos('V-min','Real')
                lower = np.array([H_min, S_min, V_min])

                H_max = cv2.getTrackbarPos('H-max','Real')
                S_max = cv2.getTrackbarPos('S-max','Real')
                V_max = cv2.getTrackbarPos('V-max','Real')
                upper = np.array([H_max, S_max, V_max])
                if(self.deteccion(lower, upper, self.tam_defecto, img)):
                    print("Objeto detectado")
            else:
                maleza_detectada = self.deteccion(self.lower_maleza, self.upper_maleza, self.__tam_maleza, img)
                if maleza_detectada:
                    print("Maleza detectada")
                else:
                    punto_caliente_detectado = self.deteccion(self.lower_punto_caliente, self.upper_punto_caliente, self.__tam_punto_caliente, img)
                    if punto_caliente_detectado:
                        print("Punto caliente detectado")

            #Si esta activado mostrara la imagen
            if self.__mostrar_pantalla:
                cv2.rectangle(img,(0,200),(200,400),(0,255,0),3)
                cv2.imshow('Real', img)
                cv2.imshow('Procesada', self.output)

            ch = cv2.waitKey(5)
            if ch == 27:
                break
        cap.release()
        cv2.destroyAllWindows()

    def deteccion(self, lower, upper, tam_max_area, img):
        #Analisis de la imagen
        img_hsv = cv2.cvtColor(img[200:400, 0:200], cv2.COLOR_BGR2HSV)
        #Transformacion morfologica para detectar mejor
        kernel_cinco = np.ones((5,5),np.uint8);
        mascara = cv2.inRange(img_hsv, lower, upper)
        cv2.erode(mascara,kernel_cinco,iterations = 2)
        cv2.dilate(mascara,kernel_cinco,iterations = 2)
        mascara = cv2.morphologyEx(mascara, cv2.MORPH_OPEN, kernel_cinco)
        mascara = cv2.morphologyEx(mascara, cv2.MORPH_CLOSE, kernel_cinco)

        self.output = cv2.bitwise_and(img[200:400, 0:200], img[200:400, 0:200], mask = mascara)
        ret, thresh = cv2.threshold(mascara,127,255,0)
        contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        #Busqueda del contorno mas grande, si supera un tamano establecido sera considerado punto caliente
        if len(contours) > 0:
            for index in range(len(contours)):
                area = cv2.contourArea(contours[index])
                if area >= tam_max_area:
                    if self.__mostrar_pantalla:
                        cv2.drawContours(img[200:400, 0:200], contours, index, (255,0,0), -1)
                    return True
        return False



vis = vision(1, True, True)
vis.revision()
