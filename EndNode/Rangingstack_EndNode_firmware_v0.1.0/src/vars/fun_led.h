#ifndef FUN_LED_H
#define FUN_LED_H
#include <Arduino.h>

#define LED_PIN 25

extern void led_Init();
extern void led_On();
extern void led_Off();
extern void led_Flash(uint16_t flashes, uint16_t delaymS);

#endif // FUN_LED_H