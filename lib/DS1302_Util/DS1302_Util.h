#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

//Prototipo RTC
void printDateTime(const RtcDateTime &dt);

void init_RTC(void);

void printTime(void);

void getDateTime(char *datestring);

void getDate(char *fecha);

void getTime(char *hora);