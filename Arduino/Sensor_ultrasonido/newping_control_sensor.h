/**
 * @file newping_control_sensor.h
 * @brief Implementación del controlador del sensor de ultrasonido HC-SR04, usando la libreria newping.
 * @author Kevin Hernández, Ángel Gil
 */

#ifndef ROBOT_NEWPING_CONTROL_SENSOR_H_
#define ROBOT_NEWPING_CONTROL_SENSOR_H_

#include <NewPing.h>
#include <control_sensor.h>
#define maxDist 200 /*Si no se indica la distacia máxima sera 200*/
 
namespace robot{
    class sensorUltra : controladorSensor{
	private:
        NewPing usensor;
    public:
		/**
		 * @brief Primer constructor de la clase.
		 * @param triggerPin, echoPin, maxDistance
		 * Puerto de datos: trigger y echo pin.
		 * maxDistance, máxima distancia a considerar.
		 */
        sensorUltra(int trigger_pin, int echo_pin, int max_distance);
		/**
		 * @brief Segundo constructor de la clase.
		 * @param triggerPin, echoPin
		 * 		  Puerto de datos: trigger y echo pin.
		 *        Máxima distancia sera maxDist.
		 */
        sensorUltra(int trigger_pin, int echo_pin);
		/**
		 * @brief Método que retorna distancia actual que percibe el sensor.
		 * @return distancia actual.
		 */
        unsigned int getDistance();
    };
};

#endif