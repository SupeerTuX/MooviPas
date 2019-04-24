
#include "LCD_i2C.h"

uint8_t bell[8] = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t note[8] = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t clk[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t duck[8] = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t retarrow[8] = {0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

//Iniciamos el LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void initLCD(void)
{
    //Inicializa el LCD
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, bell);
    lcd.createChar(1, note);
    lcd.createChar(2, clk);
    lcd.createChar(3, heart);
    lcd.createChar(4, duck);
    lcd.createChar(5, check);
    lcd.createChar(6, cross);
    lcd.createChar(7, retarrow);

    lcd.home();

    //LCD Test    X, Y
    lcd.setCursor(7, 0);
    lcd.print("MOOVIPAS");
    lcd.setCursor(0, 2);
    lcd.printByte(2);
    lcd.print(" INICIANDO SISTEMA");
}

/**
 * @brief Imprime el mensaje principal en el lcd 
 * 
 */
void msgPrincipal(char *datestring, unsigned short contador, bool wifiStatus)
{
    lcd.clear();
    lcd.home();
    lcd.print(F("MOOVIPAS  BIENVENIDO"));

    lcd.setCursor(0, 1);

    if (contador < 10000)
        lcd.print(F("0"));
    if (contador < 1000)
        lcd.print(F("0"));
    if (contador < 100)
        lcd.print(F("0"));
    if (contador < 10)
        lcd.print(F("0"));
    lcd.print(contador);
    lcd.printByte(5);

    lcd.setCursor(7, 1);
    if (wifiStatus)
    {
        //lcd.print(F("-- SISTEMA OK"));
        lcd.print(F("-- WiFi -> OK"));
    }
    else
    {
        lcd.print(F("-- WiFi FAIL "));
    }

    //Imprime date time
    lcd.setCursor(0, 2);
    lcd.print(datestring);

    lcd.setCursor(1, 3);
    lcd.print(F("ACERQUE SU TARJETA"));
}

/**
 * @brief 
 * 
 */
void msgTargetaEncontrada(void)
{
    lcd.clear();
    lcd.home();
    lcd.setCursor(5, 0);
    lcd.print(F("MOOVIPAS"));

    lcd.setCursor(0, 2);
    lcd.print(F(" TARGETA  DETECTADA "));

    lcd.setCursor(0, 3);
    lcd.print(F("NO RETIRE SU TARJETA"));
}

/**
 * @brief 
 * 
 */
void msgMostrarSaldo(char *saldoAnterior, char *saldoActual)
{
    lcd.clear();
    lcd.home();

    lcd.setCursor(5, 0);
    lcd.print(F("MOOVIPAS"));

    lcd.setCursor(0, 1);
    lcd.print(F("SALDO ANT. $"));
    lcd.print(saldoAnterior);

    lcd.setCursor(0, 2);
    lcd.print(F("SALDO ACTUAL $"));
    lcd.print(saldoActual);

    lcd.setCursor(0, 3);
    lcd.print(F("NO RETIRE SU TARJETA"));
}

void msgSaldoInsuficiente(char *saldoActual)
{
    lcd.clear();
    lcd.home();

    lcd.setCursor(5, 0);
    lcd.print(F("MOOVIPAS"));

    lcd.setCursor(0, 1);
    lcd.print(F("SALDO INSUFICIENTE"));

    lcd.setCursor(0, 2);
    lcd.print(F("SALDO ACTUAL: $"));
    lcd.print(saldoActual);

    lcd.setCursor(0, 3);
    lcd.print(F(" RETIRE  SU TARJETA"));
}

void msgErrorLectura(void)
{
    lcd.clear();
    lcd.home();

    lcd.setCursor(5, 0);
    lcd.print(F("MOOVIPAS"));

    lcd.setCursor(0, 2);
    lcd.print(F(" ERROR  DE LECTURA"));

    lcd.setCursor(0, 3);
    lcd.print(F(" RETIRE  SU TARJETA"));
}

void msgMasterKeyEncontrada(void)
{
    lcd.clear();
    lcd.home();

    lcd.setCursor(4, 0);
    lcd.print(F("CONFIGURACION"));

    lcd.setCursor(0, 2);
    lcd.print(F("MASTER KEY DETECTADA"));

    lcd.setCursor(0, 3);
    lcd.print(F("CONFIGURANDO AP"));
}

//Muetsra la configuracion del AP
void msgConfigAP(void)
{
    lcd.clear();
    lcd.home();

    lcd.setCursor(3, 0);
    lcd.print(F("ACCES POINT OK"));

    lcd.setCursor(0, 1);
    lcd.print(F("SSID: MOOVIPAS"));

    lcd.setCursor(0, 2);
    lcd.print(F("IP: 192.168.0.4"));

    lcd.setCursor(0, 3);
    lcd.print(F("PASS: 123456789"));
}

void msgMostrarHora(char *datestring)
{
    //Imprime date time
    lcd.setCursor(0, 2);
    lcd.printByte(2);
    lcd.print(datestring);
}

void msgConectandoWiFi(void)
{
    lcd.clear();
    lcd.home();

    lcd.setCursor(2, 0);
    lcd.print(F("CONECTANDO WIFI"));

    lcd.setCursor(6, 2);
    lcd.print(F("ESPERE"));

    lcd.setCursor(5, 3);
    lcd.print(F("POR FAVOR"));
}