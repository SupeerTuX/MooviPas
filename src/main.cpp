/*
Programa pago inteligente MooviPas
Plataforma ESP32
RFID listo

Inclusion de tarjeta SD
Inclusion de EEPROM
*/

#include <Arduino.h>

#include <MFRC522.h>
#include <WiFi.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <SD.h>

//Funciones locales Utilitarias
#include <LCD_i2C.h>
#include <DS1302_Util.h>
#include <Wifi_Util.h>

#include "def.h"
#include "flash.h"
#include "eeprom_aux.h"


MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
MFRC522::MIFARE_Key key;

//Clase SPI para uso de sd
SPIClass spiSD(HSPI);

//Estructura de configuracion
config_t config;

void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial)
    ; // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  //Pin buzzer como salida
  pinMode(BUZZER, OUTPUT);

  //Inicio de programa
  Serial.println();
  Serial.println(F("Inicio de programa"));

  // Init SPI bus
  SPI.begin();

  // Inicializacion de la EEPROM
  Serial.println(F("Inicio de EEPROM"));
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println(F("Error al incializar EEPROM"));
    delay(1000);
  }
  else
  {
    Serial.println(F("EEPROM OK"));
  }

  //Inicializando la SD
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  spiSD.begin(14, 27, 26, 15);
  if (!SD.begin(15, spiSD))
  {
    Serial.println("Card failed, or not present");
    // don't do anything more
  }

  //Cargamos datos iniciales en la EEPROM
  // Serial.println(F("Cargando datos iniciales"));
  // eepromDatosIniciales();

  Serial.println(F("Leyendo configiracion"));
  //Leemos la configuracion
  EEPROM_readAnything(0, config);

  // inicializando el LCD
  initLCD();

  //Test De conexion a la red Wifi
  conectarWifi();

  //Copiando la llave de la configuracion
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = config.key[i];
  }


  mfrc522.PCD_Init();                // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Ingrese su targeta"));
}

void loop()
{
  // Buscando una nueva targeta
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Seleccionamos la targeta
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  Serial.println(F("Targeta encontrada"));

  //Validamos la tarjeta
  switch (validarTarjeta())
  {
  //Proceso de cobro correcto
  case true:
    Serial.println(F("Cobro OK"));
    Serial.println();
    break;

  //Fallo de lectura
  case false:
    Serial.println(F("Error De R/W"));
    Serial.println();
    break;

  //Saldo insuficiente
  case SALDO_INSUFICIENTE:
    Serial.println(F("Saldo insuficiente"));
    Serial.println();
    break;

  //Error desconocido
  default:
    Serial.println(F("Error Desconocido"));
    Serial.println();
    break;
  }

  //Cerramos operaciones de RFID
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}


/**
 * @brief Validacion de ID y Saldo de targeta
 * 
 * @return byte 
 */
byte validarTarjeta()
{
  char id[18];
  //Leer ID
  if (!leerID(id))
  {
    return false;
  }

  char saldo[18];
  //Leer Saldo
  if (!leerSaldo(saldo))
  {
    return false;
  }

  Serial.print(F("Saldo anterior: $"));
  Serial.println(saldo);

  //Procesamos el saldo
  if (procesoSaldo(id, saldo))
  {
    Serial.print(F("Saldo actual: $"));
    Serial.println(saldo);

    //Escribir operacion en SD
    char strPrint[50];
    sprintf(strPrint, "%s\n", id);
    appendFile(SD, "/log.txt", strPrint);

    Serial.println(F("Escribiendo en tarjeta:["));
    Serial.println(strPrint);
    Serial.println(F("]"));

    //Ecribir Saldo actual en la tarjeta
    if (writeBlock(saldo, BLOCK_SALDO, TBLOCK_SALDO))
    {
      Serial.println(F("Escritura Correcta"));
      return true;
    }
    //Error de escritura
    else
    {
      Serial.println(F("Error de escritura"));
      return false;
    }
  }
  else
  {
    //Saldo insuficiente
    Serial.println(F("Saldo insuficiente"));
    return SALDO_INSUFICIENTE;
  }
}

//Procesar saldo
byte procesoSaldo(char *id, char *saldo)
{
  //Convertir saldo de texto a numero (float)
  float nSaldo = atof(saldo);

  //Verificar si hay saldo suficiente
  if (nSaldo >= 10)
  {
    //Descontamos el costo de pasaje
    nSaldo -= config.CostoPasaje;

    //Aumentamos el contador de pasaje
    config.Contador++;

    //Escribimos la eeprom
    EEPROM_writeAnything(0, config);

    //Pasamos el saldo actual de numerico a char[]
    dtostrf(nSaldo, 4, 2, saldo);
    return true;
  }
  //Si no hay saldo suficiente
  else
  {
    return false;
  }
}

//Funcion para leer un bloque de la tarjeta RFID
byte readBlock(char *dataBlock, byte block, byte trailerBlock)
{
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);
  // Authenticate using key A
  status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("Auth() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  // Read data from the block
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  for (byte i = 0; i < 16; i++)
  {
    dataBlock[i] = buffer[i];
  }
  return true;
}

//Funcion para leer saldo de la tarjeta
byte leerSaldo(char *saldo)
{
  if (readBlock(saldo, BLOCK_SALDO, TBLOCK_SALDO))
  {
    Serial.print(F("Data: "));
    Serial.println(saldo);
    return true;
  }
  else
  {
    Serial.println(F("Error al leer SALDO"));
    return false;
  }
}

//Funcion para leer saldo de la tarjeta
byte leerID(char *id)
{
  if (readBlock(id, BLOCK_ID, TBLOCK_ID))
  {
    Serial.print(F("Data: "));
    Serial.println(id);
    return true;
  }
  else
  {
    Serial.println(F("Error al leer ID"));
    return false;
  }
}

//Escribe un bloque en la tarjeta RFID
byte writeBlock(char *dataBlock, byte block, byte trailerBlock)
{

  MFRC522::StatusCode status;
  // Authenticate using key A
  status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TBLOCK_SALDO, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("Auth() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  // Escribiendo la nueva clave en los ultimo block del sector
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(BLOCK_SALDO, (byte *)dataBlock, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  else
  {
    return true;
  }
}

/**
 * @brief Agrega el texto al final del archivo
 * 
 * @param fs 
 * @param path 
 * @param message 
 */
void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}
