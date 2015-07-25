#include <engine.h>
/*Sensor ultrasonido*/
#include <NewPing.h>
#include <control_sensor.h>
#include <newping_control_sensor.h>
/*Pantalla*/

/*Auxiliares*/
#include <promedio.h>
#define puertom_a          3
#define puertom_l          4
#define pin_trigger        14
#define pin_echo           15
#define led_maleza         13
#define led_punto_caliente 19
#define velocidad          100
#define maxdistancia_ult   100

using namespace robot;
engine Robot(puertom_l, puertom_a, velocidad, velocidad, pin_trigger, pin_echo, maxdistancia_ult, led_maleza, led_punto_caliente);

void setup() {
  Robot.inicializar();
}

void loop() {
  Robot.run();
}
