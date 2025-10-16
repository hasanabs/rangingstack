#ifndef MQTT_TASK_H
#define MQTT_TASK_H
#include <Arduino.h>

/**
 * @brief Initializing connection to NS.
 */
extern void Network_init();

/**
 * @brief Keeps the MQTT client alive.
 *
 * Continuously calls mqtt.update() with a small delay to process incoming MQTT messages
 * and maintain the connection.
 */
extern void mqttKeeper(void *parameter);

/**
 * @brief Initializing MAX_MQTT_QUEUE independent of one-shot timers used for
 * trigering time expiration of \alpha seconds (For responding end node)
 */
extern void StaticTimer_init();

/**
 * @brief Callback for incoming MQTT response messages.
 *
 * Parses the hex‐encoded payload, finds the matching respPool slot by node address, extracts
 * the data into respPool[ctr], and marks gotMQTT_resp.
 *
 * @param payload Hex string from NS act as a response of a requestcontaining 2B address + data.
 * @param size    Length of the payload string.
 */
extern void topicCallbackMQTT(const String &payload, const size_t size);

/**
 * @brief Handles “system/NodeID/request” topic over MQTT.
 *
 * - `"AT"` → replies `"OK"`: Ping-like to check if GW is online or not.
 * - `"Pro<number>"` → updates the `propone` delay and confirms or rejects invalid format.
 *
 * @param payload Command string from MQTT.
 * @param size    Length of the command.
 */
extern void onlineCheck(const String &payload, const size_t size);

/**
 * @brief Monitors Wi-Fi and MQTT connectivity.
 *
 * Runs every 60 s: if Wi-Fi or MQTT is disconnected, reconnects and re-subscribes to topics.
 */
extern void checkConnection(void *parameter);

#endif //