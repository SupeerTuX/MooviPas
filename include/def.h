#include <Arduino.h>
#include <SD.h>

//Definiciones de MFRC522
#define SECTOR_ID 0 //Sector donde se almacena el ID
#define BLOCK_ID 1  //Bloque del sector donde se almacena el ID
#define TBLOCK_ID 3 //Bloque del sector NOMBRE usado para autenticacion

#define SECTOR_SALDO 2  //Sector donde se almacena el Saldo
#define BLOCK_SALDO 8   //Bloque del sector donde se almacena el Saldo
#define TBLOCK_SALDO 11 //Bloque del sector SALDO usado para autenticacion

#define BLOCK_MASTER 60  //Block donde esta la clave maestra
#define TBLOCK_MASTER 63 //Bloque del sector MASTER usado para autenticacion

//Definiciones  generales
#define SALDO_INSUFICIENTE 2 //Codigo para saldo insuficiente
#define MASTER_KEY 3         //Codgo para identificar tarjeta maestra

#define RST_PIN 4
#define SS_PIN 2

#define BUZZER 5

// #######################################################
//              Definiciones generales
// #######################################################

#define BUZZER_OK 2   //Numero de pitidos
#define BUZZER_FAIL 4 //Numero de pitidos

// #######################################################
//       Prototipos de funciones generales
// #######################################################

/**
 * @brief Identifica si la targeta ingresada es maestra
 * 
 * @return byte True = Tarjeta identificada
 */
byte leerMaster(void);

byte leerID(char *id);

byte leerSaldo(char *saldo);

byte validarTarjeta();

byte procesoSaldo(char *saldo);

byte readBlock(char *dataBlock, byte block, byte trailerBlock);

byte writeBlock(char *dataBlock, byte block, byte trailerBlock);

//Funcion de testeo, realiza una peticion GET al setvidor MooviPas.mx
byte testGET();

byte conectarWifi();

/**
 * @brief Alarma (BUZZER) Proceso OK
 * 
 */
void alarmPass(void);

/**
 * @brief Alarma (BUZZER) Proceso FAIL
 * 
 */
void alarmFail(void);

/**
 * @brief Escribe operaciones en la sd
 * 
 */
void escribirOperacion();

/**
 * @brief: Escribe al final del archivo el texto enviado como parametro
 * 
 * @param fs: Instancia del sistema de archivos
 * @param path: Ruta absoluta al archivo de texto
 * @param message: Texto a ser escrito en el archivo de texto
 */
void appendFile(fs::FS &fs, const char *path, const char *message);

void writeFile(fs::FS &fs, const char *path, const char *message);

void readFile(fs::FS &fs, const char *path);

void deleteFile(fs::FS &fs, const char *path);

void actualizarHora(void);

byte getWifiStatus(void);