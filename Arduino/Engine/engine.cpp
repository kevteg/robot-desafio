#include "engine.h"

robot::engine::engine(int pin_motor_limpieza,     int velocidad_motor_limpieza,
                      int steps_per_revolution,   int motor_pin_1,        int motor_pin_2,
              				int motor_pin_3,            int motor_pin_4,        int velocidad_motor_avance, //Tiempo en millis
                      int trigger_pin,            int echo_pin,           int max_distan_us,
                      int led_maleza,             int led_punto_caliente, int led_iluminacion,
                      int screen_adrs,            int screen_colms,       int screen_rows) :
                      sensor_ultra(trigger_pin, echo_pin, max_distan_us),
                      promedio_distancia(max_distan_us),
                      pantalla(screen_adrs, screen_colms, screen_rows, texto_ini_pantalla),
                      motor_limpieza(pin_motor_limpieza),
                      motor_avance(motor_pin_1, motor_pin_2, motor_pin_3, motor_pin_4, direccion_robot, velocidad_motor_avance){

    /*TODO:
      Distancia recorrida desde el inicio
    */

    this->velocidad_motor_limpieza  = velocidad_motor_limpieza;
    this->velocidad_motor_avance    = velocidad_motor_avance;


    /*Configuraciones iniciales de los leds*/
    leds = new int[numero_leds];
    this->leds[LED_MALEZA]          = led_maleza;
    this->leds[LED_PUNTO_CALIENTE]  = led_punto_caliente;
    pinMode(this->leds[LED_MALEZA],         OUTPUT);
    pinMode(this->leds[LED_PUNTO_CALIENTE], OUTPUT);

    _led_encendido = new bool[numero_leds];
    for(int i = 0; i < numero_leds; i++)
      cambioLed(i, false);
    n_puntos_calientes = 0;
    tiempo_inicio = 0;
    motor_avance.setDatosDistancia(209,10);
    motor_avance.setFactorVelocidad(1);
    motor_avance.Iniciar();
    escuchando = true;
    paso_motor_avance = 1;
}

void robot::engine::inicializar(){
  /*El robot estará detenido al principio*/
  pantalla.iniciar();

  Serial.begin(baudios);
  distancia_recorrida = 0;
  cambiarEstado(e_detener, e_standby);
}

void robot::engine::run(){

  unsigned long tiempo_actual, tiempo_actual_e, tiempo_actual_i;
  unsigned long tiempo_transcurrido, tiempo_transcurrido_e, tiempo_transcurrido_i;
  int distancia_al_suelo;
  escuchar();
  if(!escuchando){
    tiempo_actual_e = millis();
    tiempo_transcurrido_e = tiempo_actual_e - tiempo_inicio_e;
    if(tiempo_transcurrido_e >= tiempo_no_escuchando*segundo)
      escuchando = true;
  }


  switch(estado_robot){
    case e_avanzar:
      motor_avance.Paso(paso_motor_avanc);
      distancia_al_suelo = promedio_distancia.add(sensor_ultra.getDistance());
      //if(DEBUG)
      //  Serial.println("Paso");

      if(distancia_al_suelo <= MUY_CERCA)
        cambiarEstado(e_detener, e_maleza);
    break;
    case e_detener:

      if(parametro_robot != e_standby){
        tiempo_actual = millis();
        tiempo_transcurrido = tiempo_actual - tiempo_inicio;
        if(parametro_robot != e_standby){
          if(tiempo_transcurrido >= tiempo_detenido*segundo)
            if(parametro_robot == e_limpiar){
              cambiarEstado(e_detener, e_standby);
            }else
              cambiarEstado(e_avanzar);
        }
      }
    break;

  }

}

void robot::engine::ejecutarComando(String comando){
    Serial.println(comando); //Para confirmar la llegada del comando
    switch(comando[1]){
      case DETENER:
        cambiarEstado(conversorCharEstado(comando[1]), conversorCharParametro(comando[3])); //Comando[3] contiene la razón por la cual se está deteniendo
      break;
      default:
        cambiarEstado(conversorCharEstado(comando[1]));
      break;
    }
}


void robot::engine::cambiarEstado(estado_r estado, parametro_r parametro){
  estado_r estado_anterior = estado_robot; //En caso que el estado no sea valido
  estado_robot = estado;
  switch(estado){
    case e_detener:{
      if(DEBUG)
        Serial.println("Detener por:");
      detener();
      escuchando = false;
      tiempo_inicio = millis();
      tiempo_inicio_e = tiempo_inicio;
      parametro_robot = parametro;
      cambioLed(LED_MALEZA, false);
      cambioLed(LED_PUNTO_CALIENTE, false);
      mostrarPantalla(parametro, distancia_recorrida);
      switch (parametro) {
        case e_maleza:
          if(DEBUG)
            Serial.println("Maleza");
          tiempo_detenido = t_maleza;
          tiempo_no_escuchando   = t_maleza + t_sin_escuchar;
          cambioLed(LED_MALEZA, true);
        break;
        case e_punto_caliente:
          n_puntos_calientes++;
          tiempo_no_escuchando   = t_punto_caliente + t_sin_escuchar;
          if(n_puntos_calientes > 1){
            if(DEBUG)
              Serial.println("Punto Caliente");
            tiempo_detenido = t_punto_caliente;
            cambioLed(LED_PUNTO_CALIENTE, true);
          }else
            cambiarEstado(e_avanzar);

        break;
        case e_limpiar:{
          if(DEBUG)
            Serial.println("Limpiar");
          tiempo_detenido = t_limpieza;
          cambioLed(LED_MALEZA, true);
          if(limpieza < max_limpieza){
            if(DEBUG){
              Serial.print("Limpieza numero: ");
              Serial.println(limpieza + 1);
            }
            tiempo_no_escuchando = t_limpieza;
            limpiarMaleza();
          }else{
            if(DEBUG)
              Serial.println("No se limpia mas");

            tiempo_no_escuchando   = t_sin_escuchar;
            limpieza = 0;
            cambiarEstado(e_avanzar);
          }
        }
        break;
        //Los parámetros standyby e inicio_salida no requieren medir tiempo ni nada
        //por ello sólo no hay una sección para ellos aquí
        default:
          escuchando = true;
          if(DEBUG)
            Serial.println("Standby");
          parametro_robot = e_standby;
        break;
      }
      //Mostrará por pantalla sí es punto caliente o cualquier maleza
      //y dirá la distancia desde el inicio
    }
    break;
    default:
      estado_robot = estado_anterior;
    break;
  }
}
void robot::engine::cambiarEstado(estado_r estado){
  estado_r estado_anterior = estado_robot; //En caso que el estado no sea valido
  estado_robot = estado;
  switch(estado){
    case e_avanzar:{
      parametro_robot = e_none;
      tiempo_inicio = millis();
      pantalla.apagar();
      cambioLed(LED_MALEZA, false);
      cambioLed(LED_PUNTO_CALIENTE, false);
      avanzar();
    }
    break;
    default:
      estado_robot = estado_anterior;
    break;
  }
}
void robot::engine::mostrarPantalla(parametro_r parametro, int distancia){
  char *texto[2];
  char dist_char[7];
  String aux_str(distancia);
  aux_str = aux_str + " cm";
  aux_str.toCharArray(dist_char, 7);
  texto[0] = dist_char;
  switch (parametro) {
    case e_maleza:
      texto[1] = "Maleza";
    break;
    case e_punto_caliente:
      texto[1] = "Punto Caliente";
    break;
    case e_limpiar:
      texto[1] = "Limpieza";
    break;
    case e_standby:
      texto[1] = "Esperando..";
    break;
  }
  pantalla.mostrar(texto);
}

robot::engine::estado_r robot::engine::conversorCharEstado(char estado){
  /*Sólo estan los parámetros que tienen versión CHAR (Los que envia la rpi como parámetro)*/
  switch(estado){
    case DETENER:
      return e_detener;
    break;
    case AVANZAR:
      return e_avanzar;
    break;
  }
}

robot::engine::parametro_r robot::engine::conversorCharParametro(char estado){
  /*Sólo están los parámetros que tienen versión CHAR (Los que envia la rpi como parámetro)*/
  switch(estado){
    case MALEZA:
      return e_maleza;
    break;
    case PUNTO_CALIENTE:
      return e_punto_caliente;
    break;
    case INICIO_SALIDA:
      return e_standby;
    break;
    case LIMPIAR:
      return e_limpiar;
    break;
  }
}

void robot::engine::limpiarMaleza(){
  motor_limpieza.setSpeed(-velocidad_motor_limpieza);
  limpieza++;
}

void robot::engine::detener(){
 //distancia_recorrida = nueva_distancia
 /*Detener motor de avance */
 paso_motor_avance = 0;
 motor_limpieza.setSpeed(velocidad_0);
}

void robot::engine::avanzar(){
  paso_motor_avance = 1;
  limpieza = 0;
}

void robot::engine::escuchar(){
  char c;
  bool salir = false;

  while(Serial.available() > 0 && !salir){
    c = Serial.read();
    if(comando[0] != DELIMITADOR_I){
      salir = true;
      comando = "";
    }
    comando += c;
  }
  if(comando.length() > 0 && comando[0] == DELIMITADOR_I && comando[comando.length() -1 ] == DELIMITADOR_F){
    if(escuchando)
      ejecutarComando(comando);
    comando = "";
  }
}

void robot::engine::enviarMensaje(String mensaje){
  String envio = DELIMITADOR_I + mensaje + DELIMITADOR_F;
  Serial.print(envio);
}

void robot::engine::cambioLed(int led, bool estado){
  if(led == LED_MALEZA || led == LED_PUNTO_CALIENTE || led == LED_ILUMINACION){
    _led_encendido[led] = estado;
    digitalWrite(leds[led], _led_encendido[led]);
  }
}
