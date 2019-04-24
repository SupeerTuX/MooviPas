#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Definicion de funciones para el lcd i2c Proyecto Moovipas

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args, BYTE);
#endif

#define countof(a) (sizeof(a) / sizeof(a[0]))

void initLCD(void);

/**
 * @brief Imprime el mensaje principal en el lcd 
 * 
 */
void msgPrincipal(char *datestring, unsigned short contador, bool wifiStatus);

void msgTargetaEncontrada(void);

void msgMostrarSaldo(char *saldoAnterior, char *saldoActual);

void msgSaldoInsuficiente(char *saldoActual);

void msgErrorLectura(void);

void msgMasterKeyEncontrada(void);

void msgConfigAP(void);

void msgMostrarHora(char *datestring);

void msgConectandoWiFi(void);
