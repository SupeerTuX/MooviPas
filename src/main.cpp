/*
Programa pago inteligente MooviPas
Plataforma ESP32
RFID listo

Inclusion de tarjeta SD
Inclusion de EEPROM
*/

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <Wire.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <SD.h>

#include <LCD_i2C.h>

#include "def.h"
#include "flash.h"
#include "eeprom_aux.h"

//const char* ssid     = "Terminales";
//const char* password = "#t3rm1n4l35";

const char *ssid = "TuXWork";
const char *password = "#TuXDevelop";

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
MFRC522::MIFARE_Key key;

//Estructura de configuracion
config_t config;

//Clase SPI para uso de sd
SPIClass spiSD(HSPI);

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

  // Init SPI bus
  SPI.begin();

  // inicializando el LCD
  initLCD();

  //Copiando la llave de la configuracion
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = config.key[i];
  }

  //Hacemos testeo de la conexion WiFi
  conectarWifi();

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

byte conectarWifi()
{
  Serial.println();
  Serial.println();
  Serial.print("Conectando a la red ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

byte testGET()
{
  const char *host = "api.moovipas.mx";
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort))
  {
    Serial.println("connection failed");
    return false;
  }

  // We now create a URI for the request
  String url = "/v1/test";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  return true;
}
