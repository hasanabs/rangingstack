#ifndef ISR_LORA_H
#define ISR_LORA_H
#include <Arduino.h>

/**
 * @brief For initializing interrupt functionalities.
 */
extern void IRQ_init();

/**
 * @brief ISR for LoRa DIO1 rising edge.
 *
 * Notifies the onReceived task (in lora_task) that a LoRa RX interrupt has occurred.
 */
extern void IRAM_ATTR onLoRaDIO1();

/**
 * @brief One-shot timer expiration callback.
 *
 * Extracts the timer’s slot index from its ID and notifies the responseHandler task with that index.
 * For instance if a node send instruction check, this timer will do countdown \alphs secs. Once
 * the timer experid, the answerResponse for that node will be trigered so that GW forward the response.
 *
 * @param xTimer Handle of the expired timer.
 */
extern void IRAM_ATTR respTimerCallback(TimerHandle_t xTimer);

#endif // ISR_LORA_H