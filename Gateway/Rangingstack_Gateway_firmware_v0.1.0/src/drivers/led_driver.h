#ifndef LED_DRIVER_H
#define LED_DRIVER_H
#include <Arduino.h>

/*This works for adafruit_qtpy_esp32s3, if you want to use other boards, you need to modify it accordingly*/
#define NEOPIXEL_POWER 38
#define NUMPIXELS 39

#define Neo_magenta 0xFF00FF
#define Neo_brown 0xA52A2A
#define Neo_blue 0x0000FF
#define Neo_red 0xFF0000
#define Neo_green 0x008000
#define Neo_off 0x000000

/**
 * @brief Initializes the NeoPixel strip’s power pin and brightness.
 */
extern void led_Init();

/**
 * @brief Sets the NeoPixel strip to a solid 24-bit RGB color.
 *
 * @param color 24-bit color in 0xRRGGBB format.
 */
extern void led_On(uint32_t color);

/**
 * @brief Turns the NeoPixel strip off.
 */
extern void led_Off();

/**
 * @brief Flashes the NeoPixel strip on and off.
 *
 * @param flashes Number of on/off cycles.
 * @param delaymS Delay in milliseconds between toggles.
 * @param color   24-bit RGB color for the “on” state.
 */
extern void led_Flash(uint16_t flashes, uint16_t delaymS, uint32_t color);

#endif // LED_DRIVER_H