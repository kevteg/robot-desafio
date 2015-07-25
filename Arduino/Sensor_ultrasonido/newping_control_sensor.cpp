#include "newping_control_sensor.h"

robot::sensorUltra::sensorUltra(int trigger_pin, int echo_pin, int max_distance) :
 controladorSensor(max_distance), usensor(trigger_pin, echo_pin, max_distance){}
robot::sensorUltra::sensorUltra(int trigger_pin, int echo_pin) :
 controladorSensor(maxDist), usensor(trigger_pin, echo_pin, max_distance){}


unsigned int robot::sensorUltra::getDistance(){
	int distance = usensor.ping_cm();
		if (distance <= 0)
			return max_distance;
	return distance;
}