#include "screen.h"
robot::screen::screen(uint8_t address, uint8_t cols, uint8_t rows):lcd(address, cols, rows){}
void robot::screen::iniciar(){
  lcd.begin();
	lcd.backlight();
}

void robot::screen::mostrar(char **texto, bool blinking){
  lcd.display();
  lcd.clear();
  for(int i = 0; i < numero_lineas; i++){
    lcd.setCursor(0, i);
    lcd.print(texto[i]);
  }
  if(blinking)
    lcd.blink();
  else
    lcd.noBlink();
}
void robot::screen::apagar(){
  lcd.noDisplay();
}
