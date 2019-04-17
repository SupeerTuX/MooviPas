
#include <def.h>

/**
 * @brief Alarma (BUZZER) Proceso OK
 * 
 */
void alarmPass(void)
{

    for (byte i = 0; i < BUZZER_OK; i++)
    {
        digitalWrite(BUZZER, HIGH);
        delay(500);
        digitalWrite(BUZZER, LOW);
        delay(500);
    }
}

/**
 * @brief Alarma (BUZZER) Proceso FAIL
 * 
 */
void alarmFail(void)
{
    for (byte i = 0; i < BUZZER_FAIL; i++)
    {
        digitalWrite(BUZZER, HIGH);
        delay(500);
        digitalWrite(BUZZER, LOW);
        delay(500);
    }
}