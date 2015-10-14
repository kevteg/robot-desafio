#include "motor_adafruit.h"

robot::motor_adafruit::motor_adafruit(int number, int speed) : motor(), motor_AD(number){
	setSpeed(speed);
}

robot::motor_adafruit::motor_adafruit(int number) : motor(), motor_AD(number){
	setSpeed(0);
}

void robot::motor_adafruit::setSpeed(int speed){
		if (speed >= 0) {
			this->speed = speed;
			motor_AD.setSpeed(speed);
			motor_AD.run(FORWARD);
		}else{
			this->speed = -speed;
			motor_AD.setSpeed(-speed);
			motor_AD.run(BACKWARD);
		}
}

inline int robot::motor_adafruit::getSpeed(){
	return this->speed;
}
