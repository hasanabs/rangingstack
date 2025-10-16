#ifndef IDENTIFIER_H
#define IDENTIFIER_H
#include <Arduino.h>

/*LoRa relatted addressing*/
extern const uint16_t NetworkID;    // NetworkID identifies this connection, needs to match value in receiver (As if same subnet)
extern const uint16_t Node_address; // Node identifier (Nodes must know the GW node address)

/*TCP/IP related parameters for Wifi and MQTT*/
extern const char *ssid;
extern const char *pass;
extern const char *mqtt_server;
extern const uint16_t mqtt_port;

// MQTT topics
extern const char *parentTopic;
extern const char *topicReq;
extern const char *topicRes;
extern const char *topicResult;
extern const char *rssiResult;

#endif // IDENTIFIER_H