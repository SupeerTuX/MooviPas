
#include "Wifi_Util.h"

//const char *ssid = "TuXWork";
//const char *password = "#TuXDevelop";

/**
 * @brief Conecta a la red wifi con el SSID y en PASSWORD proporcionados
 * 
 * @param ssid: SSID de la red wifi a conectar 
 * @param pass: PASSWORD de la red wifi
 * @return byte true = OK, false = Fail
 */
byte conectarWifi(char *ssid, char *pass)
{
  //5 segundos para conectarse a la red

  Serial.println();
  Serial.println();
  Serial.print("Conectando a la red ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  unsigned long timeout = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (millis() - timeout > 20000)
    {
      Serial.println("Error al conectar a la red");
      return false;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

/**
 * @brief Configura el acces point con el ssid y password proporcionado
 * 
 * @param ssidAP: SSID del acces point 
 * @param passwordAP: password del acces point
 * @return byte  false = fail, true = ok
 */
byte cfgAccesPoint(char *ssidAP, char *passwordAP)
{
  WiFiServer server(80);
  WiFi.softAP(ssidAP, passwordAP);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");
  return true;
}

void cfgInit()
{
}

//Test de conexion
byte TestConnexion()
{
  // Use WiFiClient class to create TCP connections
  const char *host = "api.moovipas.mx";
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort))
  {
    Serial.println("connection failed");
    return false;
  }
  else
  {
    Serial.println("connection OK");
    client.stop();
    return true;
  }
}

/**
 * @brief Test de conexiona a la API del sistema
 * 
 * @return byte 
 */
byte testGET(void)
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

byte httpPOSTString(String payload)
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
  String url = "/v1/saldo";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Authorization: 3d524a53c110e4c22463b10ed32cef9d\r\n" +
               "Connection: close\r\n"
               "Content-Length: " +
               payload.length() + "\r\n\r\n" + payload + "\n\r");

  //salida de depuracion
  Serial.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Authorization: 3d524a53c110e4c22463b10ed32cef9d\r\n" +
               "Connection: close\r\n"
               "Content-Length: " +
               payload.length() + "\r\n\r\n" + payload + "\n\r");

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

byte httpPOST(char *payload)
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
  String url = "/v1/saldo";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Authorization: 3d524a53c110e4c22463b10ed32cef9d\r\n" +
               "Connection: close\r\n"
               "Content-Length: " +
               strlen(payload) + "\r\n\r\n" + payload + "\n\r");

  //salida de depuracion
  Serial.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Authorization: 3d524a53c110e4c22463b10ed32cef9d\r\n" +
               "Connection: close\r\n"
               "Content-Length: " +
               strlen(payload) + "\r\n\r\n" + payload + "\n\r");

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