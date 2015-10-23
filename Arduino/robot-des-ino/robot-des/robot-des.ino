
/*Motor*/
#include <AFMotor.h>
#include <Stepper.h>
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


//Motores:
//Avance
#define _steps_per_revolution     200
#define _motor_pin_1              1
#define _motor_pin_2              2
#define _motor_pin_3              3
#define _motor_pin_4              4
#define _velocidad_motor_avance   60
//Limpieza
#define _pin_motor_limpieza       2
#define _velocidad_motor_limpieza 255
//Sensor ultrasonido
#define _trigger_pin              14
#define _echo_pin                 19
#define _max_distan_us            30 //Distancia de maleza al suelo
//Leds
#define _led_maleza               5
#define _led_punto_caliente       6
#define _led_iluminacion          7
//Pantalla
#define _screen_adr               0x27
#define _screen_colms             16
#define _screen_rows              2

using namespace robot;
engine Robot(_pin_motor_limpieza, _velocidad_motor_limpieza, 
             _steps_per_revolution, _motor_pin_1, _motor_pin_2,
             _motor_pin_3, _motor_pin_4, _velocidad_motor_avance, 
             _trigger_pin, _echo_pin, _max_distan_us, 
             _led_maleza, _led_punto_caliente, _led_iluminacion,
             _screen_adr, _screen_colms, _screen_rows);

void setup() {
  Robot.inicializar();
}

void loop(){
  Robot.run();
}
