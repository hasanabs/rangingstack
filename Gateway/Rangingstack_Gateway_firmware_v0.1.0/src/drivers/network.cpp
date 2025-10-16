#include <Arduino.h>
#include "network.h"
#include "led_driver.h"
#include "../identifier.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <MQTTPubSubClient.h>
#include <stdio.h>

// Flags
bool fl_connect = false; // flag whether wifi connect or no

// Network and MQTT objects
WiFiClient client;
MQTTPubSubClient mqtt;

// MQTT Topics
String fullReqTopic = String(parentTopic) + "/" + String(NetworkID) + "/" + String(topicReq);
String fullResTopic = String(parentTopic) + "/" + String(NetworkID) + "/" + String(topicRes);
String fullResultTopic = String(parentTopic) + "/" + String(NetworkID) + "/" + String(topicResult);
String fullRssiTopic = String(parentTopic) + "/" + String(NetworkID) + "/" + String(rssiResult);

RespCtx respPool[MAX_MQTT_QUEUE];

void konek(WiFiClient &client, MQTTPubSubClient &mqtt, bool &status)
{
    char buff[50];
    randomSeed(esp_random());
    WiFi.begin(ssid, pass);
    Serial.println("Connecting WIFI");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println("Connected to WIFI");
    led_Flash(10, 50, Neo_green);
    Serial.println("Connecting MQTT broker address");
    while (!client.connect(mqtt_server, mqtt_port))
    {
        Serial.print(".");
        delay(3000);
    }
    Serial.println();
    Serial.println("Connected to MQTT broker address");
    status = true;
    mqtt.begin(client);
    Serial.println("Joining MQTT broker");
    sprintf(buff, "Ranging_Stack_Gw_%d", random(0, 10000));
    while (!mqtt.connect(buff, "", ""))
    {
        Serial.print(".");
        delay(3000);
    }
    Serial.println();
    Serial.println("Joined to MQTT broker");
}