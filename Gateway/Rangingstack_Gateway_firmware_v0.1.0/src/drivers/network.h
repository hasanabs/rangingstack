#ifndef NETWORK_H
#define NETWORK_H
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MQTTPubSubClient.h>
#include "lora_driver.h"

// Network and MQTT objects
extern WiFiClient client;
extern MQTTPubSubClient mqtt;

// Flags
extern bool fl_connect; // flag whether wifi connect or no

// MQTT Topics
extern String fullReqTopic;
extern String fullResTopic;
extern String fullResultTopic;
extern String fullRssiTopic;

constexpr int MAX_MQTT_QUEUE = 10; // A pool number of queue available for alpha period (e.g., 3 sec)

/**
 * @brief Struct for queueing response from MQTT
 */
struct RespCtx
{ /* Static queue for MQTT response */
    bool inUse;
    bool gotMQTT_resp;
    uint16_t nodeAddr;
    uint8_t payload[RXBUFFER_SIZE - 2];
    size_t len;
};
extern RespCtx respPool[MAX_MQTT_QUEUE];

/**
 * @brief Connects to Wi-Fi and the MQTT broker.
 *
 * - Initializes Wi-Fi with `ssid`/`pass` and waits for connection.
 * - Connects the MQTT client to `mqtt_server:mqtt_port`.
 * - Sets `status = true` once both links are up.
 *
 * @param client Reference to a WiFiClient.
 * @param mqtt   Reference to an MQTTPubSubClient.
 * @param status Reference to a bool flag updated on success.
 */
extern void konek(WiFiClient &client, MQTTPubSubClient &mqtt, bool &status);

#endif // NETWORK_H