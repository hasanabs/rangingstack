#include "led_driver.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

/*This works for adafruit_qtpy_esp32s3, if you want to use other boards, you need to modify it accordingly*/

void led_Init()
{
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);
    pixels.begin();
    pixels.setBrightness(50); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void led_On(uint32_t color)
{
    pixels.fill(color);
    pixels.show();
}

void led_Off()
{
    pixels.fill(0x000000);
    pixels.show();
}

void led_Flash(uint16_t flashes, uint16_t delaymS, uint32_t color)
{
    uint16_t index;
    for (index = 1; index <= flashes; index++)
    {
        pixels.fill(color);
        pixels.show();
        delay(delaymS);
        pixels.fill(0x000000);
        pixels.show();
        delay(delaymS);
    }
}
