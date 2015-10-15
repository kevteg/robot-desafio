#include "engine.h"

robot::engine::engine(int pin_motor_limpieza,     int pin_dir_motor_avance,
                      int steps_per_round,        int pin_step_motor_avance,
                      int velocidad_motor_avance, int velocidad_motor_limpieza,
                      int trigger_pin,            int echo_pin,
                      int max_distan_us,          int led_maleza,   int led_punto_caliente, int led_iluminacion,
                      int screen_adrs,            int screen_colms, int screen_rows) :
                      sensor_ultra(trigger_pin, echo_pin, max_distan_us),
                      promedio_distancia(max_distan_us),
                      pantalla(screen_adrs, screen_colms, screen_rows){
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
    motores[MOTOR_AVANCE]           = new motor_step(steps_per_round, pin_step_motor_avance, pin_dir_motor_avance);
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
}

void robot::engine::inicializar(){
  /*El robot estará detenido al principio*/
  //pantalla.iniciar(); //La pantalla está inicializada pero aún no se implementa
  Serial.begin(baudios);
  distancia_recorrida = 0;
  pantalla.iniciar();
  cambiarEstado(e_detener, e_standby);
}

void robot::engine::run(){
  unsigned long tiempo_actual;
  unsigned long tiempo_transcurrido;
  int distancia_al_suelo;

  //Serial.println(distancia_al_suelo);
 //if(avanzando)
    //(static_cast <motor_step *> (motores[MOTOR_AVANCE]))->individualStep();
  escuchar();

  switch(estado_robot){
    case e_avanzar:
    distancia_al_suelo = promedio_distancia.add(sensor_ultra.getDistance());
    tiempo_actual = millis();
    tiempo_transcurrido = tiempo_actual - tiempo_inicio;
    Serial.println(distancia_al_suelo);
    if(tiempo_transcurrido > t_espera_verificacion*segundo) //Debido a que este sensor se comporta extraño
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
  Serial.println("Entro");
  switch(estado){
    case e_detener:{
      Serial.println("Detener");
      detener();
      tiempo_inicio = millis();
      parametro_robot = parametro;
      mostrarPantalla(parametro, distancia_recorrida);
      switch (parametro) {
        case e_maleza:
          pantalla.mostrar("Maleza", false);
          tiempo_detenido = t_maleza;
          cambioLed(LED_MALEZA, true);
        break;
        case e_punto_caliente:
          pantalla.mostrar("Punto Caliente", false);
          tiempo_detenido = t_punto_caliente;
          cambioLed(LED_PUNTO_CALIENTE, true);
        break;
        case e_limpiar:{
          pantalla.mostrar("Limpieza", false);
          tiempo_detenido = t_limpieza;
          cambioLed(LED_MALEZA, true);
          if(limpieza <= max_limpieza)
            limpiarMaleza();
          else{
            limpieza = 0;
            cambiarEstado(e_avanzar);
          }
        }
        break;
        //Los parámetros standyby e inicio_salida no requieren de medir tiempo ni nada
        //por ello sólo no hay una sección para ellos aquí
        default:
          cambioLed(LED_MALEZA, false);
          cambioLed(LED_PUNTO_CALIENTE, false);
          pantalla.mostrar("Esperando", false);
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
      pantalla.mostrar("Avanzando", false);
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
/*  lcd.setCursor(0,0);//primera linea
  lcd.print(razon);
  lcd.setCursor(0,2);//segunda linea
  lcd.print(distancia);*/
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
  Serial.println("limpiar");
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

  (motores[MOTOR_LIMPIEZA])->setSpeed(velocidad_0);
  /* Arrancar motor de avance */
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
