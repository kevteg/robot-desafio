#include "engine.h"

robot::engine::engine(int pin_motor_limpieza, int pin_motor_avance, int velocidad_motor_avance, int velocidad_motor_limpieza, int trigger_pin, int echo_pin, int max_distan_us, int led_maleza, int led_punto_caliente) :
sensor_ultra(trigger_pin, echo_pin, max_distan_us),
promedio_distancia(max_distan_us){
  /**Aquí se deben llamar directamente a los constructores de:
    *Motor avance
    *Motor de limpieza
    *Sensor ultrasonido (Ya se llama arriba)
    *Pantalla HOLA HOLA
    */
    /*Configuraciones iniciales de los leds*/
    this->led_maleza = led_maleza;
    this->led_punto_caliente = led_punto_caliente;
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
  cambiarEstado(e_detener, INICIO_SALIDA);
  Serial.begin(baudios);
}

void robot::engine::run(){
  unsigned long tiempo_actual;
  unsigned long tiempo_transcurrido;
  int distancia_al_suelo = promedio_distancia.add(sensor_ultra.getDistance());;


  switch(estado_robot){
    case e_avanzar:
      escuchar();
      if(distancia_al_suelo > MUY_CERCA)
        cambiarEstado(e_detener, MALEZA);
    break;
    case e_detener:
    case e_limpiar:
      tiempo_actual = millis();
      tiempo_transcurrido = tiempo_actual - tiempo_inicio;
      if(tiempo_transcurrido >= tiempo_detenido)
          cambiarEstado(e_avanzar, '0');
    break;
  }

}

void robot::engine::ejecutarComando(String comando){
    switch(comando[1]){
      case DETENER:
        cambiarEstado(conversorCharEstado(comando[1]), comando[3]);
      break;
      case AVANZAR:
        cambiarEstado(conversorCharEstado(comando[1]), '0');
      break;
    }
}


void robot::engine::cambiarEstado(estado_r estado, char parametro){
  estado_r estado_anterior = estado_robot; //En caso que el estado no sea valido
  estado_robot = estado;
  switch(estado){
    case e_detener:{
      detener();
      tiempo_inicio = millis();
      if(parametro != INICIO_SALIDA){
        if(parametro == MALEZA) //Lo indica o el sensor
          tiempo_detenido = t_maleza;
        else if(parametro == PUNTO_CALIENTE)
          tiempo_detenido = t_punto_caliente;
        mostrarPantalla(parametro, distancia_recorrida);
      }
      //Mostrará por pantalla sí es punto caliente o cualquier maleza
      //y dirá la distancia desde el inicio
    }
    break;
    case e_avanzar:{
      avanzar();
    }
    break;
    case e_limpiar:{
      if(limpieza <= max_limpieza)
        limpiar_maleza();
      else{
        limpieza = 0;
        cambiarEstado(e_avanzar, NULL);
      }
    }
    break;
    default:
      estado_robot = estado_anterior;
    break;
  }
}
void robot::engine::mostrarPantalla(char razon, int distancia){

}
robot::engine::estado_r robot::engine::conversorCharEstado(char estado){
  estado_r est;
  switch(estado){
    case LIMPIAR:
      est = e_limpiar;
    break;
    case DETENER:
      est = e_detener;
    break;
    case AVANZAR:
      est = e_avanzar;
    break;
  }
  return est;
}

void robot::engine::limpiar_maleza(){
  cambiarEstado(e_detener, MALEZA);
  limpieza++;
  enviarMensaje((String)LIMPIAR + (String)SEPARADOR + String(limpieza));
  /*
  El activar o desactivar el motor de limpieza se hará en run
  */
}

void robot::engine::detener(){
 //distancia_recorrida = nueva_distancia
 /*Detener motor de avance
 motor_principal.detener()*/
}

void robot::engine::avanzar(){
  //Se reinicia ésta variable porque si era mayor a cero
  //es porque estaba limpiando
  limpieza = 0;
  /*Arrancar motor de avance
  motor_principal.avanzar()*/
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
    digitalWrite(!led?led_maleza:led_punto_caliente, _led_encendido[led]?HIGH:LOW);
  }
}
