#include "motor_step.h"

robot::motor_step::motor_step(int steps_per_round, int pin_step, int pin_dir){
  this->steps_per_round = steps_per_round;
  this->pin_dir  = pin_dir;
  this->pin_step = pin_step;
  pinMode(pin_step, OUTPUT);
  pinMode(pin_dir, OUTPUT);
  digitalWrite(pin_step, LOW);
  digitalWrite(pin_dir, LOW);
  setSpeed(0);
}

void robot::motor_step::setSpeed(int speed){
  this->speed = speed;
  digitalWrite(pin_dir, (this->speed)?HIGH:LOW);
}

void robot::motor_step::individualStep(){
  digitalWrite(pin_step, HIGH);
  sleep();
  digitalWrite(pin_step, LOW);
  sleep();
}

void robot::motor_step::sleep(){
  Serial.println("sleep");
  long hold_time_us = (long)(1.0 / (double) steps_per_round / speed / 2.0 * 1E6);
  int overflow_count = hold_time_us / 65535;
  for (int i = 0; i < overflow_count; i++)
    delayMicroseconds(65535);

  delayMicroseconds((unsigned int) hold_time_us);
}

inline int robot::motor_step::getSpeed(){
  return this->speed;
}
