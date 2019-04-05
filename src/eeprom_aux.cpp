 #include <Arduino.h>
 #include "eeprom_aux.h"

void eepromDatosIniciales(void)
{
  config_t config;
  //Terminal por defecto
  strcpy(config.TerminalID, "T001");
  //AutobusID por defecto
  strcpy(config.AutobusID, "M-122");
  //SSID Por defecto
  //config.SSID = "TuXWork";
  strcpy(config.SSID, "TuXWork");
  
  //Password Por defecto de la RED
  //config.Password = "#TuXDevelop";
  strcpy(config.Password, "#TuXDevelop");
  
  //Servidor de red
  //config.Server = "api.ustgm.net";
  strcpy(config.Server, "api.moovipas.mx");
  
  //Costo del pasaje por defecto
  config.CostoPasaje = 10;

  //Contador de pasajes
  config.Contador = 0;
  
  //llave por defecto
  config.key[0] = 0x84; config.key[1] = 0x8F; config.key[2] = 0x69;
  config.key[3] = 0xBC; config.key[4] = 0xF2; config.key[5] = 0xEB;
  //Escribiendo datos de la estructura en la EEPROM
  EEPROM_writeAnything(0, config);
}


