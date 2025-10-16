#include "lora_tasks.h"
#include <Arduino.h>
#include <SX128XLT.h>
#include "isr_lora.h"
#include "../Settings.h"
#include "../identifier.h"
#include "../utils/consts.h"
#include "../drivers/network.h"
#include "../drivers/lora_driver.h"
#include "../utils/debug_utils.h"

unsigned long detik;
uint16_t bufDestAddr;
uint8_t RX_queue_count = 0;
int16_t propone = 50; // duration for propone the response for the end node request (in ms)

// FreeRTOS handlers
TaskHandle_t onReceivedHandle = NULL;
TaskHandle_t responseHandlerHandle = NULL;
TimerHandle_t timers[MAX_MQTT_QUEUE];
StaticTimer_t timerBuffers[MAX_MQTT_QUEUE];

void onReceived(void *pvParameters)
{
    int16_t PacketRSSI;
    uint8_t *bufOpcode = nullptr;
    uint8_t *buf_NS_to_Node = nullptr;
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for interrupt
        // Do the heavy work triggered by IRQ
        uint16_t irqStatus = LT.readIrqStatus();
        if (irqStatus & IRQ_RX_DONE)
        {
            PacketOK = LT.receivedReliableAutoACK_addr_handler(RXBUFFER, RXBUFFER_SIZE, NetworkID, Node_address, ACKdelay, static_cast<int8_t>(TXpower_set), &bufDestAddr);
            LT.clearIrqStatus(IRQ_RADIO_ALL);
            if (PacketOK > 0)
            {
                // timer start (ms) , then call forward to NS
                detik = millis();

                // Pull out opcode, packet length & payload pointer *before* doing anything else
                bufOpcode = RXBUFFER;           // first byte = opcode
                RXPacketL = LT.readRXPacketL(); // total PHY packet length
                RXPayloadL = RXPacketL - 8;     // strip LoRa headers
                buf_NS_to_Node = RXBUFFER + 1;  // payload starts immediately after opcode

                // If this packet *needs* a delayed response, stash it in the pool + start its timer
                if (*bufOpcode == OP_TASK_CHK || *bufOpcode == OP_CFG_CHK) // condition where the GW need to asign a task to reply end node
                {
                    for (int i = 0; i < MAX_MQTT_QUEUE; i++)
                    {
                        if (!respPool[i].inUse)
                        {
                            respPool[i].inUse = true;
                            respPool[i].gotMQTT_resp = false;
                            respPool[i].nodeAddr = bufDestAddr;
                            // fire off its one-shot timer
                            xTimerChangePeriod(timers[i], pdMS_TO_TICKS(ListenDelay - propone), 0);
                            xTimerStart(timers[i], 0);
                            // for debug/queue‐tracking
                            RX_queue_count++;
                            break;
                        }
                    }
                }
                PacketRSSI = LT.readPacketRSSI(); // read the received packets RSSI value
                mqtt.publish(fullRssiTopic, String(PacketRSSI));
                debugPrintln("R"); // Simple received indicator
                // debugerPrinter(bufDestAddr, bufOpcode, buf_NS_to_Node); // To see the payload's info
                LoRa_to_NS(mqtt, bufDestAddr, *bufOpcode, buf_NS_to_Node, RXPayloadL - 1); // bufDestAddr is the future (for now means received data from)
            }
            openReceiveWindow();
        }
    }
}

void answerResponse(void *pvParameters)
{
    /* Remember, data structure => 2B address + 1B opcode + rest data
      RespPool[idx].payload is data but without 2B of address */
    uint32_t idx;
    while (1)
    {
        // Wait indefinitely for any timer callback to notify
        if (xTaskNotifyWait(0, 0xFFFFFFFF, &idx, portMAX_DELAY) != pdTRUE)
            continue;

        if (!respPool[idx].gotMQTT_resp)
        {
            debugPrintln("Can't access NS");
            goto cleanup;
        }

        if (respPool[idx].payload[0] == OP_NULL)
        {
            debugPrintln("No packet for node " + String(respPool[idx].nodeAddr));
            goto cleanup;
        }

        /* At this point, payload is valid and can be sent to node */
        detachInterrupt(DIO1); // detach IRQ, kill listen window, send data to end node, reopen listen window, re-attach IRQ
        xSemaphoreTake(LoRa_module_accessed_sem, portMAX_DELAY);
        Packet_to_LoRa(LT, respPool[idx].nodeAddr, Node_address, respPool[idx].payload, respPool[idx].len);
        xSemaphoreGive(LoRa_module_accessed_sem);
        printPayloadBytes(respPool[idx].payload, respPool[idx].len); // For debuging, if needed
        respPool[idx].payload[0] = OP_NULL;                          // for safety only
        respPool[idx].nodeAddr = 0x0000;                             // for safety only (expected no node with address of 0)
        openReceiveWindow();
        attachInterrupt(DIO1, onLoRaDIO1, RISING);

    // Clean up and mark slot as free
    cleanup:
        if (RX_queue_count >= 0)
            RX_queue_count--;
        respPool[idx].inUse = false;
        respPool[idx].gotMQTT_resp = false;
        debugPrintln("");
    }
}