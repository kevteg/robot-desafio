/**
 * @file engine.h
 * @brief Librería de control del robot del lado de Arduino
 * @author Kevin Hernández, Gerardo Sánchez
 */
//<> en lugar de "" porque las librerias propias se tratan como externas
#ifndef ROBOT_ENGINE_H_
#define ROBOT_ENGINE_H_

/*Motor*/
#include <motor.h>
#include <AFMotor.h>
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
#include "promedio.h"

/*Definiciones*/
#define MUY_CERCA                 20   //Si se supera esta distancia es maleza al suelo
#define max_distancia_ultrasonido 100  //Máxima distancia por defecto del ultrasonido en cm
#define t_punto_caliente          10   //Tiempo de espera para punto caliente
#define t_maleza                  10   //Tiempo por el que estará detenido
#define t_limpieza                10   //Tiempo por el que estará limpiando
#define t_espera_verificacion     2   //Tiempo de espera de verificación
#define t_inicio                  5   //tiempo inicio
#define tiempo_por_paso           0.5   //Tiempo de espera de verificación
#define segundo                   1000 //Segundos en milisegundos
#define numero_leds               3    //Número de leds del robot por defecto
#define baudios                   9600 //Baudios de la comunicación serial
#define max_limpieza              3    //Máxima cantidad que se limpiara
#define LED_MALEZA                0    //Posición cero de vector de leds será el led maleza
#define LED_PUNTO_CALIENTE        1    //Posición uno de vector de leds será el led de punto caliente
#define LED_ILUMINACION           2    //Posición uno de vector de leds será el led de punto caliente
/*Estas variables se usan para enviar mensajes a la raspberry pi y para manejor interno*/
//Revisar:
#define INICIO                    'I'
#define LIMPIAR                   'L'  //Limpiar en el protócolo
#define AVANZAR                   'A'  //Avanzar en el protócolo
#define DETENER                   'D'  //Detener en el protócolo
#define MALEZA                    'M'  //Maleza en el protócolo
#define PUNTO_CALIENTE            'P'  //Punto caliente para indicar que el robot se detiene por esa razón
#define INICIO_SALIDA             'X'  //Para indicarle al robot al detenerse que no importa la distancia porque es inicio o salida
#define SEPARADOR                 ':'  //Separador del protócolo
#define DELIMITADOR_I             '<'  //Delimitador inicial del protócolo
#define DELIMITADOR_F             '>'  //Delimitador final del protócolo
#define n_motores                  2   //Número de motores
#define MOTOR_LIMPIEZA             0   //Posición del motor de limpieza en el vector de motores
#define MOTOR_AVANCE               1   //Posición del motor de avance en el vector de motores
#define velocidad_0                0   //Velocidad cero de avance
#define texto_ini_pantalla         "Heimdal: "
#define DEBUG                      true
#define t_sin_escuchar             3
#define direccion_robot            1
#define                       paso_motor_avanc 1
namespace robot{
    class engine{
		private:
			sensorUltra               sensor_ultra;
			promedioDinamico <int, 3> promedio_distancia;
      int                       *leds;
      bool                      *_led_encendido;                    //Variable que indica si esta o no un led encendido
			unsigned long             tiempo_inicio;                      //Tiempo de inicio para verificar las rutinas
      unsigned long             tiempo_inicio_e;                    //Tiempo de inicio para verificar cuando se oye a la rpi
      unsigned long             tiempo_inicio_m;                //Tiempo de inicio para verificar cuando se oye a la rpi
      String                    comando;                            //Comandos que llegan al robot
      int                       limpieza;                           //Variable que cuenta el número de veces que se ha limpiado
                                                                    //Debe reiniciarse cada vez que se llega al máximo de número de limpieza
      int                       pin_motor_limpieza;
      int                       distancia_recorrida;                //Distancia desde el origen
      int                       tiempo_detenido;                    //Tiempo por el cual estará detenido
      int                       velocidad_motor_avance;
      int                       velocidad_motor_limpieza;

      motor_adafruit            motor_limpieza;
      MotorHS                   motor_avance;
      screen                    pantalla;
      bool                      escuchando;
      bool                      avanzando;
      int                       tiempo_no_escuchando;
      int                       n_puntos_calientes;
      int                       paso_motor_avance;

    protected:
      /*Protegidos para que la misma clase pueda tener métodos que retornen el tipo*/
      enum estado_r{
              e_detener,
              e_avanzar
              //e_inicio
              };
      enum parametro_r{
              e_maleza,
              e_punto_caliente,
              e_limpiar,
              e_standby, // Para cuando el robot estaba limpiando y termina, se queda en standby hasta que se le confirma si se termino de limpiar la maleza
              e_none
              };
      parametro_r parametro_robot; //Parámetro de robot (Para conocer porque está defenido)
      estado_r    estado_robot; //El estado en el que iniciará por defecto debería ser detenido hasta que la Raspberry pi indique lo contrario
		public:
			/**
			 * @brief Primer constructor de la clase.
			 * @param pin_motor_limpieza: Puerto a conectar el motor del método de limpieza
       *      pin_motor_limpieza: Pin en la shield del motor de limpieza
			 *		  pin_dir_motor_avance: pin de dirección del motor de avance
       *      steps_per_round:      Steps por vuelta del motor de avance
       *      pin_step_motor_avance: Pin de step del motor de avance
			 *		  velocidad_motor_avance: Velocidad de los motores de avance
       *      velocidad_motor_limpieza: Velocidad de los motores de limpieza
			 *  	  trigger_pin: Puerto trigger del sensor de ultrasonido
			 *		  echo_pin: Puerto echo del sensor de ultrasonido
			 *	    max_distan_us: Máxima distancia a medir por el sensor de ultrasonido
			 *		  pin_sir1: Pin del sensor infrarojo 1
			 *      pin_sir2: Pin del sensor infrarojo 2
       *      screen_adrs: Dirección de pantalla (Normalemente 0x27)
       *      screen_colms: Columnas de pantalla
       *      screen_rows: Filas de pantalla
			 */
			engine(int pin_motor_limpieza,     int velocidad_motor_limpieza,
             int steps_per_revolution,   int motor_pin_1,  int motor_pin_2,
             int motor_pin_3,            int motor_pin_4,  int velocidad_motor_avance,
             int trigger_pin,            int echo_pin,
             int max_distan_us,          int led_maleza,   int led_punto_caliente, int led_iluminacion,
             int screen_adrs,            int screen_colms, int screen_rows);

      /**
      * @brief Interpretar los comandos que se reciben para cambiar el estado del robot
      *	@param comando: comando a ejecutar por el robot
      */
      void ejecutarComando(String comando);
      /**
      * @brief Cambia la rutina actual usando el comando recibido
      * @param estado: nueva rutina
      *        parámetro: parametro de la rutina (Para detenerse)
      * @return true si es una rutina correcta
      */
      void cambiarEstado(estado_r estado, parametro_r parametro);
      void cambiarEstado(estado_r estado);
      /**
      * @brief Conversor de estado en char que se recibe por comando a estado_r interno
      * @param estado: nueva rutina
      * @return mismo estado pero en formato interno
      */
      robot::engine::estado_r conversorCharEstado(char estado);
      /**
      * @brief Conversor de parametro en char que se recibe por comando a parametro_r interno
      * @param estado: nueva rutina
      * @return mismo parametro pero en formato interno
      */
      robot::engine::parametro_r    conversorCharParametro(char estado);

      /**
			 * @brief Rutina de limpieza
       * Detiene el robot y activa el motor de limpieza durante el tiempo designado por variable estátiva max_limpieza
       * El número de limpieza en la que va se lleva con la variable limpieza
			 */
      void limpiarMaleza();
      /**
			 * @brief Detener el motor principal del robot
       * Detiene el movimiento del robot
			 */
      void detener();
      /**
			 * @brief Robot avanza
			 */
      void avanzar();

      /**
			 * @brief Escuchar ordenes del raspberry pi, es en esta función donde se decide cambiar el estado del robot
			 */
			void escuchar();
      /**
      * @brief Enviar mensaje al controlador del robot
      * @param mensaje: mensaje a enviar a la computadora
      */
      void enviarMensaje(String mensaje);
			/**
			 * @brief Inicializar el estado del robot
			 */
			void inicializar();
      /**
      * @brief Cambiar el estado de led
      * @param led: Si led está encendido se apaga y si está apagado se enciende
      */
      void cambioLed(int led, bool estado);
      /**
      * @brief Mostrar información por pantalla
      * @param mensaje: Lo que se debe mostrar, la idea es que sea un mensaje
      */
      void mostrarPantalla(parametro_r parametro, int distancia);
      /**
      * @brief actualizar el estado del robot según la lectura del sensor de ultrasonido o según lo que reciba por el xBee
      * 		  debe ser llamado repetidamente
      */
      void run();

    };
};
#endif
