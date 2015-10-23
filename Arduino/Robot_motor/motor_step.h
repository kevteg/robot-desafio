/**
 * @file motor_step.h
 * @brief Controlador del motor step
 * @author Kevin Hernández, Gerardo Sánchez
 */
#ifndef ROBOT_STEP_H
#define ROBOT_STEP_H

#include <Arduino.h>
#include <motor.h>
#include <Stepper.h>

	namespace robot{

		class motor_step : public motor{
			private:
				int motor_pin_1;
				int motor_pin_2;
				int motor_pin_3;
				int motor_pin_4;
				int steps_per_revolution;
				int speed; //Velocidad en RPM
				Stepper step_m;
			public:
				/**
				 * @brief Primer constructor de la clase.
				 * @param number_of_steps: Los steps que hará en cada ronda el motor
				 * pines de motor
				 */
				motor_step(int steps_per_revolution, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, int speed);

				/**
				 * @brief Cambiar la velocidad actual del motor
				 * @param speed: nueva velocidad
				 */
				void setSpeed(int speed);
        /**
				 * @brief Un paso individual,
         *  se llama todo el tiempo en el engine del robot
				 *  @param dir: true adelante, false atrás
				 */
        void individualStep(bool dir);
				/*
				 * @brief Obtener la velocidad actual.
				 * @return velocidad actual
				 */
				inline int getSpeed();
		};

	};

#endif
