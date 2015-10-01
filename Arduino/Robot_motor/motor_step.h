/**
 * @file motor_step.h
 * @brief Controlador del motor step
 * @author Kevin Hernández, Gerardo Sánchez
 */
#ifndef ROBOT_STEP_ADAFRUIT_H
#define ROBOT_STEP_ADAFRUIT_H

#include <Arduino.h>
#include <motor.h>

	namespace robot{

		class motor_step : public motor{
			private:
				int speed;
        int steps_per_round;
        int pin_step;
        int pin_dir;
			public:
				/**
				 * @brief Primer constructor de la clase.
				 * @param steps_per_round: Los steps que hará en cada ronda el motor
         * @param pin_step: Pin step del motor
         * @param pin_dir: Pin dir del motor
				 */
				motor_step(int steps_per_round, int pin_step, int pin_dir);

				/**
				 * @brief Cambiar la velocidad actual del motor
				 * @param speed: nueva velocidad
				 */
				void setSpeed(int speed);
        /**
				 * @brief Un paso individual,
         *  se llama todo el tiempo en el engine del robot
				 */
        void individualStep();
        /**
         * @brief Separación entre el encendido y apagado del pin step
         *        se calcula en función de la velocidad
         */
        void sleep();
				/**
				 * @brief Obtener la velocidad actual.
				 * @return velocidad actual
				 */
				inline int getSpeed();
		};

	};

#endif
