#include "motor_adafruit.h"

robot::motor_adafruit::motor_adafruit(int number, int speed) : motor(), motor_AD(number){
	setSpeed(speed);
}

robot::motor_adafruit::motor_adafruit(int number) : motor(), motor_AD(number){
	setSpeed(0);
}

void robot::motor_adafruit::setSpeed(int speed){
	this->speed = speed;
	/*Al dividir la velocidad por sí misma sé si es positiva o negativa
		*Si es negativa: (-1)(-speed) = speed
		*Si es positiva: (1)(speed)   = speed
	*/
	motor_AD.setSpeed((this->speed/this->speed)*this->speed);
	motor_AD.run((this->speed >= 0)?FORWARD:BACKWARD);
}

inline int robot::motor_adafruit::getSpeed(){
	return this->speed;
}
