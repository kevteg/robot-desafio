#include "screen.h"
robot::screen::screen(uint8_t address, uint8_t cols, uint8_t rows, char *texto_ini) : lcd(address, cols, rows), text_ini(texto_ini){}
void robot::screen::iniciar(){
  lcd.begin();
	lcd.backlight();
}

void robot::screen::mostrar(char **texto){
  lcd.display();
  lcd.clear();
  lcd.print(text_ini);
  for(int i = 0; i < numero_lineas; i++){
    lcd.setCursor((strlen(text_ini)*(numero_lineas - 1 - i)), i);
    lcd.print(texto[i]);
  }
}
void robot::screen::apagar(){
  lcd.noDisplay();
}
