/**
 * @file control_sensor.h
 * @brief Definición de los sensores de distancia.
 * @author Kevin Hernández, Ángel Gil
 */
#ifndef ROBOT_CONTROL_SENSOR_H_
#define ROBOT_CONTROL_SENSOR_H_
 
namespace robot{
    class controladorSensor{
    protected:
        unsigned int max_distance;
		
    public:
        /**
          * @brief Constructor de la clase.
          * @param distance, la distancia máxima a evaluar.
          */
        controladorSensor(unsigned int distance) : max_distance(distance) {}

        /**
         * @brief Retornar la distancia al objeto mas cercano.
         * @return Retorna la distancia al objeto mas cercano o la distancia máxima sino se ha detectado ningún objeto.
         */
        virtual unsigned int getDistance() = 0;

    };
};
#endif