#include "motor_step.h"

robot::motor_step::motor_step(int steps_per_revolution, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, int speed) :
motor_pin_1(motor_pin_1),
motor_pin_2(motor_pin_2),
motor_pin_3(motor_pin_3),
motor_pin_4(motor_pin_4),
steps_per_revolution(steps_per_revolution),
speed(speed),
step_m(steps_per_revolution, motor_pin_1, motor_pin_2, motor_pin_3, motor_pin_4){

  setSpeed(this->speed);
}

/**
 * @brief Cambiar la velocidad actual del motor
 * @param speed: nueva velocidad
 */
void robot::motor_step::setSpeed(int speed){
  this->speed = speed;
  step_m.setSpeed(this->speed);
}
/**
 * @brief Un paso individual,
 *  se llama en el tiempo en el engine del robot
 */
void robot::motor_step::individualStep(bool dir){
  //+- 1
  step_m.step((dir?1:-1));
}
/*
 * @brief Obtener la velocidad actual.
 * @return velocidad actual
 */
inline int robot::motor_step::getSpeed(){
  return this->speed;
}
