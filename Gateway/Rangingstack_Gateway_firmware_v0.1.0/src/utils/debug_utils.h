#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H
#include <Arduino.h>
#include "../Settings.h"

#define debugPrint(x)        \
    do                       \
    {                        \
        if (DEBUG_MODE)      \
            Serial.print(x); \
    } while (0)
#define debugPrintHex(x)          \
    do                            \
    {                             \
        if (DEBUG_MODE)           \
            Serial.print(x, HEX); \
    } while (0)
#define debugPrintln(x)        \
    do                         \
    {                          \
        if (DEBUG_MODE)        \
            Serial.println(x); \
    } while (0)

#endif // DEBUG_UTILS_H