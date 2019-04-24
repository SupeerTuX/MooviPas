#include <Arduino.h>
#include <WiFi.h>

/**
 * @brief Funcion para conectar a la red Wifi
 * 
 * @return byte 
 */
byte conectarWifi(char *ssid, char *pass);

/**
 * @brief Test de conexiona a la API del sistema
 * 
 * @return byte 
 */
byte testGET(void);

/**
 * @brief Configura el acces point con el ssid y password proporcionado
 * 
 * @param ssidAP: SSID del acces point 
 * @param passwordAP: password del acces point
 * @return byte  false = fail, true = ok
 */
byte cfgAccesPoint(char *ssidAP, char *passwordAP);

byte httpPOST(char *payload);

byte httpPOSTString(String payload);

void cfgInit();