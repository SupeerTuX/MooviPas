

#include "DS1302_Util.h"

//RTC
ThreeWire myWire(17, 16, 3); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void init_RTC(void)
{
    //Test RTC
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    Rtc.Begin();

    // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // printDateTime(compiled);
    // Serial.println();

    // if (Rtc.GetIsWriteProtected())
    // {
    //     Serial.println("RTC was write protected, enabling writing now");
    //     Rtc.SetIsWriteProtected(false);
    // }

    // if (!Rtc.GetIsRunning())
    // {
    //     Serial.println("RTC was not actively running, starting now");
    //     Rtc.SetIsRunning(true);
    // }

    // RtcDateTime now = Rtc.GetDateTime();
    // if (now < compiled)
    // {
    //     Serial.println("RTC is older than compile time!  (Updating DateTime)");
    //     Rtc.SetDateTime(compiled);
    // }
    // else if (now > compiled)
    // {
    //     Serial.println("RTC is newer than compile time. (this is expected)");
    // }
    // else if (now == compiled)
    // {
    //     Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    // }
}

void printTime(void)
{
    //Imprimir tiempo
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime &dt)
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
               dt.Second());
    Serial.print(datestring);
}

void printDateTimeString(const RtcDateTime &dt, char *date)
{
    char datestring[20];
    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
               dt.Day(),
               dt.Month(),
               dt.Year(),
               dt.Hour(),
               dt.Minute(),
               dt.Second());
    //Serial.print(datestring);
    strcpy(date, datestring);
}

void printDateString(const RtcDateTime &dt, char *hora)
{
    char datestring[20];
    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u.%02u.%04u"),
               dt.Day(),
               dt.Month(),
               dt.Year());
    Serial.print(datestring);
    strcpy(hora, datestring);
}

void printTimeString(const RtcDateTime &dt, char *hora)
{
    char datestring[20];
    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u:%02u:%02u"),
               dt.Hour(),
               dt.Minute(),
               dt.Second());
    Serial.print(datestring);
    strcpy(hora, datestring);
}

void getDateTime(char *date)
{
    RtcDateTime now = Rtc.GetDateTime();

    printDateTimeString(now, date);
    //Serial.println();

    //delay(10000); // ten seconds
}

void getDate(char *fecha)
{
    RtcDateTime now = Rtc.GetDateTime();
    printDateString(now, fecha);
    //Serial.println();

    //delay(10000); // ten seconds
}

void getTime(char *hora)
{
    RtcDateTime now = Rtc.GetDateTime();
    printTimeString(now, hora);
    //Serial.println();

    //delay(10000); // ten seconds
}