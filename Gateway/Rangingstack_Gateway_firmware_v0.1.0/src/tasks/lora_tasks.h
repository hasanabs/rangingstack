#ifndef LORA_TASKS_H
#define LORA_TASKS_H
#include <Arduino.h>

extern unsigned long detik; // For time measurement only
extern uint8_t RX_queue_count;
extern int16_t propone;

// FreeRTOS handlers
extern TaskHandle_t onReceivedHandle;
extern TaskHandle_t responseHandlerHandle;
extern TimerHandle_t timers[];
extern StaticTimer_t timerBuffers[];

/**
 * @brief Main LoRa RX processing task.
 *
 * Waits for DIO1 notifications, reads IRQ status and packet data, if valid, forwards packets
 * to the network server via LoRa_to_NS(), and schedules delayed responses if required.
 */
extern void onReceived(void *pvParameters);

/**
 * @brief Handles all delayed responses to end nodes.
 *
 * Blocks on timer notifications, checks for a matching MQTT response in respPool[idx], and if
 * the respond is valid, sends it over LoRa, then cleans up the slot and updates RX_queue_count.
 */
extern void answerResponse(void *pvParameters);

#endif // LORA_TASKS_H