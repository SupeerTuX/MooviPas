#include <Arduino.h>
#include <WiFi.h>


/**
 * @brief Funcion para conectar a la red Wifi
 * 
 * @return byte 
 */
byte conectarWifi(void);

/**
 * @brief Test de conexiona a la API del sistema
 * 
 * @return byte 
 */
byte testGET(void);