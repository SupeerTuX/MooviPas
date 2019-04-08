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
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <SD.h>

#include "def.h"
#include "flash.h"
#include "eeprom_aux.h"


#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

//const char* ssid     = "Terminales";
//const char* password = "#t3rm1n4l35";

const char* ssid     = "TuXWork";
const char* password = "#TuXDevelop";

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;

//Iniciamos el LCD
LiquidCrystal_I2C lcd(0x27,20,4); 

//RTC
//ThreeWire myWire(27,26,25); // IO, SCLK, CE
ThreeWire myWire(25,33,32); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

//Estructura de configuracion
config_t config;

//Clase SPI para uso de sd
SPIClass spiSD(HSPI);

//Prototipo RTC
void printDateTime(const RtcDateTime& dt);

void setup() {
  Serial.begin(115200);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  //Inicio de programa
  Serial.println();
  Serial.println();
  Serial.println(F("Inicio de programa"));

  // Inicializacion de la EEPROM
  Serial.println(F("Inicio de EEPROM"));
  if (!EEPROM.begin(EEPROM_SIZE)) {
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
  if (!SD.begin(15, spiSD)) {
    Serial.println("Card failed, or not present");
    // don't do anything more
  }

  Serial.println("card initialized.");
  Serial.println("Abriendo Archivo");
  File dataFile = SD.open("/datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("Test");
    dataFile.close();
    // print to the serial port too:
    Serial.println("Test");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  //Cargamos datos iniciales en la EEPROM
  // Serial.println(F("Cargando datos iniciales"));
  // eepromDatosIniciales();

  Serial.println(F("Leyendo configiracion"));
  //Leemos la configuracion 
  EEPROM_readAnything(0, config);
  // Configuracion leida
  Serial.print(F("Autobus ID"));
  Serial.println(config.AutobusID);

  // Init SPI bus
  SPI.begin();      

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


  //Test RTC
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }




  //Llave inicial
  key.keyByte[0] = 0x84;
  key.keyByte[1] = 0x8F;
  key.keyByte[2] = 0x69;
  key.keyByte[3] = 0xBC;
  key.keyByte[4] = 0xF2;
  key.keyByte[5] = 0xEB;

  //Hacemos testeo de la conexion WiFi
  conectarWifi();

  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Ingrese su targeta"));
}

void loop() {
  // Buscando una nueva targeta
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Seleccionamos la targeta
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //Imprimir tiempo
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  Serial.println();

  Serial.println(F("Targeta encontrada"));

  //Validamos la tarjeta
  switch(validarTarjeta())
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

  testGET();

  //Cerramos operaciones de RFID
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}


byte validarTarjeta()
{
  char id[18];
  //Leer ID
  if (!leerID(id))
  { return false; }

  char saldo[18];
  //Leer Saldo
  if (!leerSaldo(saldo))
  { return false; }

  Serial.print(F("Saldo anterior: $")); Serial.println(saldo);

  //Procesamos el saldo
  if (procesoSaldo(id, saldo))
  {
    Serial.print(F("Saldo actual: $")); Serial.println(saldo);

    //Ecribir Saldo actual en la tarjeta
    if (writeBlock(saldo, BLOCK_SALDO, TBLOCK_SALDO))
    {
      Serial.println(F("Escritura Correcta"));
      return true;
    }
    //Error de escritura
    else{
      Serial.println(F("Error de escritura"));
      return false;
    }
  }
  else{
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
    //nSaldo -= cfg.CostoPasaje;
    //Pasamos el saldo actual de numerico a char[]

    //Aumentamos el contador de pasaje
    //cfg.Contador++;
    //Escribimos la eeprom
    //EEPROM_writeAnything(0, cfg);

    dtostrf(nSaldo, 4, 2, saldo);
    return true;
  }
  //Si no hay saldo suficiente
  else{
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
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Auth() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  // Read data from the block
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
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
  else{
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
  else{
    Serial.println(F("Error al leer ID"));
    return false;
  }
}

//Escribe un bloque en la tarjeta RFID
byte writeBlock(char *dataBlock, byte block, byte trailerBlock)
{

  MFRC522::StatusCode status;
   // Authenticate using key A
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TBLOCK_SALDO, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Auth() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  // Escribiendo la nueva clave en los ultimo block del sector
  //Serial.print(F("[MOVIIPASS] WR Saldo")); Serial.println(nSaldo);
  //Serial.print(F("[")); Serial.print(strPrint); Serial.print(F("]"));
  //dump_byte_array((byte *)strPrint, 16); Serial.println();
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(BLOCK_SALDO, (byte*)dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  else {
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

  while (WiFi.status() != WL_CONNECTED) {
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
  const char* host = "api.moovipas.mx";
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
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
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  return true;

}


//RTC AUX

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}


