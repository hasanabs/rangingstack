#include "fun_led.h"
#include <Arduino.h>

// LED stuff
void led_Init()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}
void led_On()
{
    digitalWrite(LED_PIN, HIGH);
}
void led_Off()
{
    digitalWrite(LED_PIN, LOW);
}
void led_Flash(uint16_t flashes, uint16_t delaymS)
{
    uint16_t index;
    for (index = 1; index <= flashes; index++)
    {
        led_On();
        delay(delaymS);
        led_Off();
        delay(delaymS);
    }
}
