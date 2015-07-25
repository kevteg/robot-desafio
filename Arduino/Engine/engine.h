/**
 * @file engine.h
 * @brief Librería de control del robot del lado de Arduino
 * @author Kevin Hernández, Gerardo Sánchez
 */

#ifndef ROBOT_ENGINE_H_
#define ROBOT_ENGINE_H_
/*Movilidad*/
/**La clase de movilidad debe adaptarse al motor que tenga
* Habrán dos instancias:
*    Una para el motor de avance y otro para el motor de limpieza
*/
  /*Sensor ultrasonido*/
  #include <NewPing.h>
  #include <control_sensor.h>
  #include <newping_control_sensor.h>
  /*Pantalla*/

  /*Auxiliares*/
  #include <promedio.h>
/*Definiciones*/
#define MUY_CERCA                 20   //Si se supera esta distancia es maleza
#define max_distancia_ultrasonido 100  //Máxima distancia por defecto del ultrasonido en cm
#define t_punto_caliente          10   //Tiempo de espera para punto caliente
#define t_maleza                  10   //Tiempo por el que estará detenido
#define segundo                   1000 //Segundos en milisegundos
#define numero_leds               2    //Número de leds del robot por defecto
#define baudios                   9600 //Baudios de la comunicación serial
#define max_limpieza              3    //Máxima cantidad que se limpiara
#define LED_MALEZA                0    //Posición cero de vector de leds será el led maleza
#define LED_PUNTO_CALIENTE        1    //Posición uno de vector de leds será el led de punto caliente
/*Estas variables se usan para enviar mensajes a la raspberry pi y para manejor interno*/
#define LIMPIAR                   'L'  //Limpiar en el protócolo
#define AVANZAR                   'A'  //Evadir en el protócolo
#define DETENER                   'D'  //Detener en el protócolo
#define MALEZA                    'M'  //Maleza en el protócolo
#define PUNTO_CALIENTE            'P'  //Punto caliente para indicar que el robot se detiene por esa razón
#define INICIO_SALIDA             'X'  //Para indicarle al robot al detenerse que no importa la distancia porque es inicio o salida
#define SEPARADOR                 ':'  //Separador del protócolo
#define DELIMITADOR_I             '<'  //Delimitador inicial del protócolo
#define DELIMITADOR_F             '>'  //Delimitador inicial del protócolo
namespace robot{
    class engine{
		private:
			sensorUltra sensor_ultra;
			promedioDinamico <int, 3> promedio_distancia;
      int led_maleza;
      int led_punto_caliente;
      bool *_led_encendido;            //Variable que indica si esta o no un led encendido
			unsigned long tiempo_inicio;    //Tiempo de inicio para verificar las rutinas
      String comando;                 //Comandos que llegan al robot
      int limpieza;                   //Variable que cuenta el número de veces que se ha limpiado
                                      //Debe reiniciarse cada vez que se llega al máximo de número de limpieza
      int distancia_recorrida;        //Distancia desde el origen
      int tiempo_detenido;            //Tiempo por el cual estará detenido
    protected:
      /*Protegidos para que la misma clase pueda tener métodos que retornen el tipo*/
      enum estado_r{
              e_limpiar,
              e_detener,
              e_avanzar
              };
      estado_r estado_robot; //El estado en el que iniciará por defecto debería ser detenido hasta que la Raspberry pi indique lo contrario
		public:
			/**
			 * @brief Primer constructor de la clase.
			 * @param pin_motor_limpieza: Puerto a conectar el motor del método de limpieza
			 *		  pin_motor_avance: Puerto a conectar el motor de avance
			 *		  velocidad_motor_avance: Velocidad de los motores de avance
       *      velocidad_motor_limpieza: Velocidad de los motores de limpieza
			 *  	  trigger_pin: Puerto trigger del sensor de ultrasonido
			 *		  echo_pin: Puerto echo del sensor de ultrasonido
			 *	    max_distan_us: Máxima distancia a medir por el sensor de ultrasonido
			 *		  pin_sir1: Pin del sensor infrarojo 1
			 *      pin_sir2: Pin del sensor infrarojo 2
			 */
			engine(int pin_motor_limpieza, int pin_motor_avance, int velocidad_motor_avance, int velocidad_motor_limpieza, int trigger_pin, int echo_pin, int max_distan_us, int led_maleza, int led_punto_caliente);

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
      void cambiarEstado(estado_r estado, char parametro);
      /**
      * @brief Conversor de estado en char que se recibe por comando a estado_r interno
      * @param estado: nueva rutina
      * @return mismo estado pero en formato interno
      */
      robot::engine::estado_r conversorCharEstado(char estado);
      /**
			 * @brief Rutina de limpieza
       * Detiene el robot y activa el motor de limpieza durante el tiempo designado por variable estátiva max_limpieza
       * El número de limpieza en la que va se lleva con la variable limpieza
			 */
      void limpiar_maleza();
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
      void cambioLed(int led);
      /**
      * @brief Mostrar información por pantalla
      * @param mensaje: Lo que se debe mostrar, la idea es que sea un mensaje
      */
      void mostrarPantalla(char razon, int distancia);
      /**
      * @brief actualizar el estado del robot según la lectura del sensor de ultrasonido o según lo que reciba por el xBee
      * 		  debe ser llamado repetidamente
      */
      void run();

    };
};
#endif
