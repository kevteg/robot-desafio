
/*Motor*/
#include <AFMotor.h>
#include <Stepper.h>
#include <motor.h>
#include <motor_adafruit.h>
#include <MotorHS.h>

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
#define _motor_pin_1              5
#define _motor_pin_2              6
#define _motor_pin_3              9
#define _motor_pin_4              10
#define _velocidad_motor_avance    2//millis
//Limpieza
#define _pin_motor_limpieza       1
#define _velocidad_motor_limpieza 255
//Sensor ultrasonido
#define _trigger_pin              15
#define _echo_pin                 14
#define _max_distan_us            100 //Distancia maxima sensor us
//Leds
#define _led_punto_caliente       17
#define _led_maleza               16
#define _led_iluminacion          0
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
