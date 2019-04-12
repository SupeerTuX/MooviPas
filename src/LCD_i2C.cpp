

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include <def.h>

//Iniciamos el LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void initLCD(void)
{
    //Inicializa el LCD
    lcd.init();
    lcd.backlight();
    lcd.home();

    //LCD Test
    lcd.print("Hello world...");
    lcd.setCursor(0, 1);
    lcd.print(" i ");
    lcd.printByte(3);
    lcd.print(" arduinos!");
}