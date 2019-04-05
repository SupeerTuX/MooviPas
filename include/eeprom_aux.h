#include <EEPROM.h>
#include <Arduino.h>

#define   EEPROM_SIZE   128

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
  const byte* p = (const byte*)(const void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);

    EEPROM.commit();
  return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
  byte* p = (byte*)(void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}




//Estructura de datos destinados a ser almacenados en la eeprom
struct config_t
{
  char TerminalID[10];
  char AutobusID[10];
  char SSID[15];
  char Password[15];
  char Server[30];
  float CostoPasaje;
  unsigned short Contador;
  byte key[6];
};



//Funcion para cargar datos iniciales a la eeprom
void eepromDatosIniciales(void);