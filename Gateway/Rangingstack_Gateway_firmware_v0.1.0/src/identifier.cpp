#include "identifier.h"

/*LoRa relatted addressing*/
const uint16_t NetworkID = 0x3210;    // NetworkID identifies this connection, needs to match value in receiver (As if same subnet)
const uint16_t Node_address = 0xFFFF; // Node identifier (Nodes must know the GW node address)

/*TCP/IP related parameters for Wifi and MQTT (For non TLS only),
For TLS, certificate is needed and the broker need to support TLS too*/
const char *ssid = "YourSSID";
const char *pass = "YourWifiPass";
const char *mqtt_server = "MQTT_server_IP_or_URL";
const uint16_t mqtt_port = 1883;

/*MQTT topics
The structure of the topic will be: {parentTopic}/{NetworkID}/{subtopic}
Example:  rangstack/12816/tag_req or rangstack/12816/tag_res, etc
Note: 0x3210 = 12816 decimal
*/
const char *parentTopic = "rangstack";
const char *topicReq = "tag_req";
const char *topicRes = "tag_res";
const char *topicResult = "tag_result";
const char *rssiResult = "RSSI";