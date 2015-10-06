/**
 * @file control_motor.h
 * @brief Definición básica del controlador del motor del robot.
 * @author Kevin Hernández, Gerardo Sánchez
 */
#ifndef ROBOT_MOTOR_H_
#define ROBOT_MOTOR_H_

		namespace robot{

			class motor{
				public:
					/**
					 * @brief Cambiar velocidad del motor.
					 * @param speed, la nueva velocidad del robot.
					 *  Valores correctos entre -255 y 255 (Inclusive)
					 *  Velocidad > 0, motor va hacia adelante.
					 *  Velocidad < 0, motor va hacia atras.
					 *  Velocidad = 0, detiene el motor
					 */
					 virtual void setSpeed(int speed){};

					/**
					 * @brief Retornar la velidad actual del robot.
					 * @return La velocidad actual en un rango de -255 a 255.
					 */
					 virtual int getSpeed(){};
			};

		};

#endif
