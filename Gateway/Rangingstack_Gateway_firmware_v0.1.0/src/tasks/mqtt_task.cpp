#include "mqtt_task.h"
#include <Arduino.h>
#include "isr_lora.h"
#include "lora_tasks.h"
#include "../identifier.h"
#include "../drivers/network.h"
#include "../utils/hex_utils.h"
#include "../drivers/lora_driver.h"

void Network_init()
{
    mqtt.setOptions(65535, true, 1000);
    fl_connect = false;
    konek(client, mqtt, fl_connect); // connect to network and MQTT broker
    mqtt.subscribe(fullResTopic, topicCallbackMQTT);
    mqtt.subscribe("system/" + String(NetworkID) + "/request", onlineCheck);
}

void StaticTimer_init()
{
    // — create MAX_MQTT_QUEUE independent of one-shot timers, statically —
    for (int i = 0; i < MAX_MQTT_QUEUE; i++)
    {
        respPool[i].inUse = false;
        timers[i] = xTimerCreateStatic(
            "RespTmr",
            pdMS_TO_TICKS(1),     // dummy, we’ll change per‐use
            pdFALSE,              // one-shot
            (void *)(uintptr_t)i, // ID = index into respPool
            respTimerCallback,
            &timerBuffers[i]);
    }
}

void mqttKeeper(void *parameter)
{
    while (1) // Ping MQTT (to check if there is new message or no)
    {
        mqtt.update();
        vTaskDelay(100);
    }
}

void topicCallbackMQTT(const String &payload, const size_t size)
{
    if (RX_queue_count > 0) // If LoRa Received
    {
        uint32_t ctr = 0;
        uint16_t responseAddress = extractAddrFromHexString(payload); // Slot for the address of MQTT response for comparison purpose
        do
        {
            if (respPool[ctr].nodeAddr == responseAddress)
                break;
            ctr++;
        } while (ctr < MAX_MQTT_QUEUE);
        if (ctr >= MAX_MQTT_QUEUE)
        {
            Serial.println("No matching requestor address");
        }
        else
        {
            uint8_t NS_byte_len = (payload.length() / 2); // Received from string (e.g: FA is a byte but 2 characters)
            respPool[ctr].len = NS_byte_len - 2;
            extractRestFromHexString(payload, respPool[ctr].payload, (size_t)NS_byte_len); // put it into queue
            respPool[ctr].gotMQTT_resp = true;
        }
    }
}

void onlineCheck(const String &payload, const size_t size) // For checking if the GW is still active or no from MQTT
{
    if (payload == "AT")
    {
        mqtt.publish("system/" + String(NetworkID) + "/response", "OK");
    }
    else if (payload.startsWith("Pro"))
    {
        String numStr = payload.substring(3); // Extract part after "Pro"
        bool isValidNumber = true;

        // Check if every character is a digit or '-'
        for (size_t i = 0; i < numStr.length(); i++)
        {
            char c = numStr[i];
            if (!(isdigit(c) || (i == 0 && c == '-')))
            { // Allow '-' only at the start
                isValidNumber = false;
                break;
            }
        }

        if (isValidNumber && numStr.length() > 0)
        {                             // Ensure it's not just "Pro"
            propone = numStr.toInt(); // Convert to integer
            mqtt.publish("system/" + String(NetworkID) + "/response", "Propone changed to " + numStr);
        }
        else
        {
            mqtt.publish("system/" + String(NetworkID) + "/response", "Invalid Pro format (expected Pro[number]) in one line");
        }
    }
}

void checkConnection(void *parameter)
{
    while (1)
    {
        if (WiFi.status() != WL_CONNECTED) // Wifi connection checker (reconnect if lost connection)
        {
            fl_connect = false;
        }
        else if (WiFi.status() == WL_CONNECTED && fl_connect == false)
        {
            konek(client, mqtt, fl_connect); // konek inside here to avoid reconnect function called  repetitively everytime lost conn
            mqtt.subscribe(fullResTopic, topicCallbackMQTT);
            mqtt.subscribe("system/" + String(NetworkID) + "/request", onlineCheck);
        }
        vTaskDelay(60000);
    }
}
