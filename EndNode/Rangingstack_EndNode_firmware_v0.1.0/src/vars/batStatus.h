#ifndef BATSTATUS_H
#define BATSTATUS_H
#include <Arduino.h>

extern uint8_t bat_level;
extern void bat_status_init(void);
extern uint8_t read_bat_status();

#endif