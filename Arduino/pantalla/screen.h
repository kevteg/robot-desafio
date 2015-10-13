/**
 * @file screen.h
 * @brief Definición básica del controlador de pantalla bajo interfaz i2c
 * @author Kevin Hernández, Gerardo Sánchez
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#ifndef SCREEN_H
#define SCREEN_H

		namespace robot{

			class screen{
      private:
        LiquidCrystal_I2C lcd;
				public:
          screen(uint8_t address, uint8_t cols, uint8_t rows);
					/**
					 * @brief Iniciar pantalla
					 */
					void iniciar();
          /**
					 * @brief Mostrar texto en pantalla
           * @param texto: texto a mostrar por pantalla
           *        blinking: Para que el mensaje titile hasta que se envie otra cosa
					 */
          void mostrar(char texto[], bool blinking);
			};

		};

#endif
