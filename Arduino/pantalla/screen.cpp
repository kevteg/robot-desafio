#include "screen.h"
robot::screen::screen(uint8_t address, uint8_t cols, uint8_t rows):lcd(address, cols, rows){}
void robot::screen::iniciar(){
  lcd.begin();
	lcd.backlight();
}

void robot::screen::mostrar(char texto[], bool blinking){
  lcd.clear();
  lcd.print(texto);
  if(blinking)
    lcd.blink();
  else
    lcd.noBlink();
}
