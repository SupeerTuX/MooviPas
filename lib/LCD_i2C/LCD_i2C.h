#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

//Definicion de funciones para el lcd i2c Proyecto Moovipas

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args, BYTE);
#endif

void initLCD(void);