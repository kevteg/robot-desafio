
/*Motor*/
#include <AFMotor.h>
#include <motor.h>
#include <motor_adafruit.h>
#include <motor_step.h>

/*Sensor ultrasonido*/
#include <NewPing.h>
#include <control_sensor.h>
#include <newping_control_sensor.h>
/*Pantalla*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <screen.h>

/*Auxiliares*/
#include <promedio.h>
/*Principal*/
#include <engine.h>

#define _pin_motor_limpieza 2
#define _pin_dir_motor_avance 11
#define _steps_per_round 200
#define _pin_step_motor_avance 13
#define _velocidad_motor_avance 4
#define _velocidad_motor_limpieza 255
#define _trigger_pin 14
#define _echo_pin 19
#define _max_distan_us 6
#define _led_maleza 5
#define _led_punto_caliente 6


using namespace robot;
engine Robot(_pin_motor_limpieza, _pin_dir_motor_avance, _steps_per_round, _pin_step_motor_avance, _velocidad_motor_avance, _velocidad_motor_limpieza, 
            _trigger_pin, _echo_pin, _max_distan_us, 
            _led_maleza, _led_punto_caliente);

void setup() {
  Robot.inicializar();
}

void loop() {
  Robot.run();
}
