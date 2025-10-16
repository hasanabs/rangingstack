#include "isr_lora.h"
#include <Arduino.h>
#include "lora_tasks.h"
#include "../Settings.h"

void IRQ_init()
{
    pinMode(DIO1, INPUT);
    attachInterrupt(DIO1, onLoRaDIO1, RISING);
}

void IRAM_ATTR onLoRaDIO1()
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    vTaskNotifyGiveFromISR(onReceivedHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Called when any of the static timers expires
void IRAM_ATTR respTimerCallback(TimerHandle_t xTimer)
{
    uint32_t idx = (uint32_t)(uintptr_t)pvTimerGetTimerID(xTimer);
    BaseType_t xHigher;
    // Notify the single response handler, passing idx in the notification value
    xTaskNotifyFromISR(responseHandlerHandle, idx, eSetValueWithOverwrite, &xHigher);
    portYIELD_FROM_ISR(xHigher);
}