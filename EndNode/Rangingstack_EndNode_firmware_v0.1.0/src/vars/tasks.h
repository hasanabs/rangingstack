#ifndef TASKS_H
#define TASKS_H
#include <Arduino.h>
#include <SPI.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <timers.h>
#include "ctl_tag_commun.h"
#include "fun_led.h"

// // SX1280 mutex
extern SemaphoreHandle_t xBlock_AOn_Rang_sem;
extern SemaphoreHandle_t xBlock_LP_Commun_sem;

// Miscelanious
extern void preSleep();
extern void afterWake();

// Listener handles
extern TaskHandle_t TaskMsgCheck_Handle;
extern TaskHandle_t TaskMsgCheck_AOn_Handle;
extern TaskHandle_t TaskListen_Handle;
extern TimerHandle_t TaskListen_TimerHandle;
extern TaskHandle_t TaskRangingSlave_AOn_Handle;

extern void TaskCfgCheck(void *pvParameters);
extern void TaskMsgCheck(void *pvParameters);
void TaskMsgCheck_AOn(void *pvParameters);

void TaskListen_CallbackTimer(TimerHandle_t xTimer);
void TaskListen(void *pvParameters);
void TaskListen_AOn(void *pvParameters);

void TaskRanging(void *pvParameters); // Ptp Ranging
void TaskAR(void *pvParameters);      // Passive ranging
void rangingMaster(uint16_t Destination_address);
void rangingSlave();
void TaskRangingSlave_AOn(void *pvParameters);
void rangingPassive(uint16_t Master_id, uint16_t Slave_id);

#endif // TASKS_H
