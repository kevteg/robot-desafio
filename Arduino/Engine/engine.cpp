#include "engine.h"

robot::engine::engine(int pin_motor_limpieza,     int velocidad_motor_limpieza,
                      int steps_per_revolution,   int motor_pin_1,  int motor_pin_2,
              				int motor_pin_3,            int motor_pin_4,  int velocidad_motor_avance,
                      int trigger_pin,            int echo_pin,
                      int max_distan_us,          int led_maleza,   int led_punto_caliente, int led_iluminacion,
                      int screen_adrs,            int screen_colms, int screen_rows) :
                      sensor_ultra(trigger_pin, echo_pin, max_distan_us),
                      promedio_distancia(max_distan_us),
                      pantalla(screen_adrs, screen_colms, screen_rows, texto_ini_pantalla){
  /**Aquí se deben llamar directamente a los constructores de:
    *Motor avance (Listo)
    *Motor de limpieza (Listo)
    *Sensor ultrasonido (Ya se llama arriba)} (Listo)
    *Pantalla
    */
    /*TODO:
      Distancia recorrida desde el inicio
    */

    motores                         = new motor*[n_motores];
    motores[MOTOR_LIMPIEZA]         = new motor_adafruit(pin_motor_limpieza);
    motores[MOTOR_AVANCE]           = new motor_step(steps_per_revolution, motor_pin_1, motor_pin_2, motor_pin_3, motor_pin_4, velocidad_motor_avance);
    this->velocidad_motor_limpieza  = velocidad_motor_limpieza;
    this->velocidad_motor_avance    = velocidad_motor_avance;

    /*Configuraciones iniciales de los leds*/
    leds = new int[numero_leds];
    this->leds[LED_MALEZA]          = led_maleza;
    this->leds[LED_PUNTO_CALIENTE]  = led_punto_caliente;
    this->leds[LED_ILUMINACION]     = led_iluminacion;
    pinMode(this->leds[LED_MALEZA],         OUTPUT);
    pinMode(this->leds[LED_PUNTO_CALIENTE], OUTPUT);
    pinMode(this->leds[LED_ILUMINACION],    OUTPUT);

    _led_encendido = new bool[numero_leds];
    for(int i = 0; i < numero_leds; i++)
      cambioLed(i, (i < numero_leds - 1)?false:true);
    tiempo_inicio = 0;
    escuchando = true;
}

void robot::engine::inicializar(){
  /*El robot estará detenido al principio*/
  Serial.begin(baudios);
  distancia_recorrida = 0;
  cambiarEstado(e_detener, e_standby);
  pantalla.iniciar();
}

void robot::engine::run(){
  unsigned long tiempo_actual, tiempo_actual_e, tiempo_actual_m;
  unsigned long tiempo_transcurrido, tiempo_transcurrido_e, tiempo_transcurrido_m;
  int distancia_al_suelo;

  if(avanzando){
    tiempo_actual_m = millis();
    tiempo_transcurrido_m = tiempo_actual_m - tiempo_inicio_m;
    if(tiempo_transcurrido_m >= tiempo_por_paso*segundo){
      if(DEBUG)
        Serial.println("Paso");
      tiempo_inicio_m = millis();
      (static_cast <motor_step *> (motores[MOTOR_AVANCE]))->individualStep();
    }
  }

  escuchar();
  if(!escuchando){
    tiempo_actual_e = millis();
    tiempo_transcurrido_e = tiempo_actual_e - tiempo_inicio_e;
    if(tiempo_transcurrido_e >= tiempo_no_escuchando*segundo)
      escuchando = true;
  }

  switch(estado_robot){
    case e_avanzar:
    distancia_al_suelo = promedio_distancia.add(sensor_ultra.getDistance());
    //tiempo_transcurrido = tiempo_actual - tiempo_inicio;
    /*if(DEBUG){
      Serial.print("Distancia al suelo: ");
      Serial.println(distancia_al_suelo);
    }*/
    //if(tiempo_transcurrido > t_espera_verificacion*segundo) //Debido a que este sensor se comporta extraño
      if(distancia_al_suelo <= MUY_CERCA)
        cambiarEstado(e_detener, e_maleza);
    break;
    /*Que este detenido o este limpiando es lo mismo*/
    case e_detener:
      /*Sólo va a esperar un tiempo si está detenido por maleza o punto caliente*/
      if(parametro_robot != e_standby){
        tiempo_actual = millis();
        tiempo_transcurrido = tiempo_actual - tiempo_inicio;
        if(parametro_robot != e_standby){
          if(tiempo_transcurrido >= tiempo_detenido*segundo)
            if(parametro_robot == e_limpiar){
              enviarMensaje((String)LIMPIAR + (String)SEPARADOR + String(limpieza));
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
      case AVANZAR:
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
          if(DEBUG)
            Serial.println("Punto Caliente");
          tiempo_detenido = t_punto_caliente;
          tiempo_no_escuchando   = t_punto_caliente + t_sin_escuchar;
          cambioLed(LED_PUNTO_CALIENTE, true);
        break;
        case e_limpiar:{
          if(DEBUG)
            Serial.println("Limpiar");
          tiempo_detenido = t_limpieza;
          cambioLed(LED_MALEZA, true);
          if(limpieza < max_limpieza){
            if(DEBUG){
              Serial.print("Limpieza número: ");
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
            Serial.println("Inicio/standby");
          cambioLed(LED_MALEZA, false);
          cambioLed(LED_PUNTO_CALIENTE, false);
          pantalla.apagar();
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
  motores[MOTOR_LIMPIEZA]->setSpeed(velocidad_motor_limpieza);
  limpieza++;
}

void robot::engine::detener(){
 //distancia_recorrida = nueva_distancia
 /*Detener motor de avance */
 avanzando = false;
 motores[MOTOR_LIMPIEZA]->setSpeed(velocidad_0);

}

void robot::engine::avanzar(){
  avanzando = true;
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
