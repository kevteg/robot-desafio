#include "engine.h"

robot::engine::engine(int pin_motor_limpieza, int pin_dir_motor_avance,
                      int steps_per_round,  int pin_step_motor_avance,
                      int velocidad_motor_avance, int velocidad_motor_limpieza,
                      int trigger_pin, int echo_pin,
                      int max_distan_us, int led_maleza, int led_punto_caliente) :
                      sensor_ultra(trigger_pin, echo_pin, max_distan_us),
                      promedio_distancia(max_distan_us){
  /**Aquí se deben llamar directamente a los constructores de:
    *Motor avance (Listo)
    *Motor de limpieza (Listo)
    *Sensor ultrasonido (Ya se llama arriba)} (Listo)
    *Pantalla
    */
    /*TODO:
      Distancia recorrida desde el inicio
    */
    motores                         = new motor[n_motores];
    motores[MOTOR_LIMPIEZA]         = motor_adafruit(pin_motor_limpieza);
    motores[MOTOR_AVANCE]           = motor_step(steps_per_round, pin_step_motor_avance, pin_dir_motor_avance);
    this->velocidad_motor_limpieza  = velocidad_motor_limpieza;
    this->velocidad_motor_avance    = velocidad_motor_avance;
    /*Configuraciones iniciales de los leds*/
    this->led_maleza                = led_maleza;
    this->led_punto_caliente        = led_punto_caliente;
    pinMode(this->led_maleza, OUTPUT);
    pinMode(this->led_punto_caliente, OUTPUT);
    digitalWrite(this->led_maleza, LOW);
    digitalWrite(this->led_punto_caliente, LOW);
    _led_encendido = new bool[numero_leds];
    for(int i = 0; i < numero_leds; i++)
      _led_encendido[i] = false;
    tiempo_inicio = 0;
}

void robot::engine::inicializar(){
  /*El robot estará detenido al principio*/
  distancia_recorrida = 0;
  cambiarEstado(e_detener, e_inicio_salida);
  Serial.begin(baudios);
}

void robot::engine::run(){
  unsigned long tiempo_actual;
  unsigned long tiempo_transcurrido;
  int distancia_al_suelo = promedio_distancia.add(sensor_ultra.getDistance());

  if(avanzando)
    (static_cast <motor_step *> (&motores[MOTOR_AVANCE]))->individualStep();

  switch(estado_robot){
    case e_avanzar:
      escuchar();
      if(distancia_al_suelo <= MUY_CERCA)
        cambiarEstado(e_detener, e_maleza);
    break;
    /*Que este detenido o este limpiando es lo mismo*/
    case e_detener:
    case e_limpiar://Revisar <---
      /*Sólo va a esperar un tiempo si está detenido por maleza o punto caliente*/
      if(parametro_robot != e_inicio_salida){
        tiempo_actual = millis();
        tiempo_transcurrido = tiempo_actual - tiempo_inicio;
        if(tiempo_transcurrido >= tiempo_detenido)
            cambiarEstado(e_avanzar);
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
  switch(estado){
    case e_detener:{
      detener();
      tiempo_inicio = millis();
      parametro_robot = parametro;
      mostrarPantalla(parametro, distancia_recorrida);
      switch (parametro) {
        case e_maleza:
          tiempo_detenido = t_maleza;
          cambioLed(LED_MALEZA);
        break;
        case e_punto_caliente:
          tiempo_detenido = t_punto_caliente;
          cambioLed(LED_PUNTO_CALIENTE);
        break;
        default:
          parametro_robot = e_none;
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
      avanzar();
    }
    break;
    case e_limpiar:{
      if(limpieza <= max_limpieza)
        limpiar_maleza();
      else{
        limpieza = 0;
        cambiarEstado(e_avanzar);
      }
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
  switch(estado){
    case LIMPIAR:
      return e_limpiar;
    break;
    case DETENER:
      return e_detener;
    break;
    case AVANZAR:
      return e_avanzar;
    break;
  }
}

robot::engine::parametro_r robot::engine::conversorCharParametro(char estado){
  switch(estado){
    case MALEZA:
      return e_maleza;
    break;
    case PUNTO_CALIENTE:
      return e_punto_caliente;
    break;
    case INICIO_SALIDA:
      return e_inicio_salida;
    break;
  }
}

void robot::engine::limpiar_maleza(){
  cambiarEstado(e_detener, e_maleza);
  motores[MOTOR_LIMPIEZA].setSpeed(velocidad_motor_limpieza);
  limpieza++;
  enviarMensaje((String)LIMPIAR + (String)SEPARADOR + String(limpieza));
}

void robot::engine::detener(){
 //distancia_recorrida = nueva_distancia
 /*Detener motor de avance */
 avanzando = false;
 motores[MOTOR_LIMPIEZA].setSpeed(velocidad_0);

}

void robot::engine::avanzar(){
  avanzando = true;
  (motores[MOTOR_LIMPIEZA]).setSpeed(velocidad_0);
  limpieza = 0;
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

void robot::engine::cambioLed(int led){
  if(led == LED_MALEZA || led == LED_PUNTO_CALIENTE){
    _led_encendido[led] = !_led_encendido[led];
    digitalWrite(!led?led_maleza:led_punto_caliente, _led_encendido[led]);
  }
}
