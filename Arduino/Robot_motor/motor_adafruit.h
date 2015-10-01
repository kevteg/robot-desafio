/**
 * @file motor_adafruit.h
 * @brief Controlador del motor usando la librería de adafruit.
 * @author Kevin Hernández, Gerardo Sánchez
 */
#ifndef ROBOT_MOTOR_ADAFRUIT_H
#define ROBOT_MOTOR_ADAFRUIT_H
/*
*Nota, lo pongo aquí (AFMotor) porque el compilador es un poco primitivo y si solo llamo esa libreria
*en un lugar (aqui o en el .ino) el cpp no reconoce al objeto de tipo AF_DCMotor
*/
#include <motor.h>
#include <AFMotor.h>


	namespace robot{

		class motor_adafruit : public motor{
			private:
				AF_DCMotor motor_AD;
				int speed;
			public:
				/**
				 * @brief Segundo constructor de la clase.
				 * @param número del motor a controlar de 1 a 4, velocidad inicial (-255 a 255).
				 */
				motor_adafruit(int number, int speed);
				/**
				 * @brief Tercer constructor de la clase.
				 * @param número del motor a controlar de 1 a 4.
				 */
				motor_adafruit(int number);
				/**
				 * @brief Cambiar la velocidad actual del motor
				 * @param speed: nueva velocidad
				 */
				void setSpeed(int speed);
				/**
				 * @brief Obtener la velocidad actual.
				 * @return velocidad actual
				 */
				inline int getSpeed();
		};

	};

#endif
