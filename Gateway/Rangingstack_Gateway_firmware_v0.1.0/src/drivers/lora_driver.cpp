#include <Arduino.h>
#include "lora_driver.h"
#include <SPI.h>
#include <SX128XLT.h>
#include <MQTTPubSubClient.h>
#include "network.h"
#include "led_driver.h"
#include "../identifier.h"
#include "../utils/consts.h"
#include "../utils/hex_utils.h"
#include "../utils/debug_utils.h"

SX128XLT LT; // create a library class instance called LT
SemaphoreHandle_t LoRa_module_accessed_sem;

// LoRa Receive variable helper
uint8_t RXBUFFER[RXBUFFER_SIZE]; // create the buffer that received packets are copied into
uint8_t RXPacketL;               // stores length of packet received
uint8_t RXPayloadL;              // stores length of payload received
uint8_t PacketOK;                // set to > 0 if packetOK
uint8_t NS_byte_len = 0;         // Temp length variable for receiving byte data from NS

void LoRa_init()
{
    SPI.begin();
    if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE) != true)
    {
        while (1)
            led_Flash(10, 50, Neo_red); // If failed to connect to SX1280, led flashes
    }
    LT.setupLoRa(communFreq, 0, communSF, communBW, communCR);
}

void printPayloadBytes(uint8_t *payload_buff, uint8_t payload_size) // for debuging
{
    for (size_t i = 0; i < payload_size; ++i)
    {
        if (payload_buff[i] < 0x10)
        {
            debugPrint("0"); // Print leading zero for single hex digits
        }
        debugPrintHex(payload_buff[i]); // Print the byte in hexadecimal format
        debugPrint(" ");                // Add a space for readability
    }
    debugPrintln();
}

void debugerPrinter(uint16_t bufDestAddr, uint8_t *bufOpcode, uint8_t *buf_NS_to_Node)
{
    debugPrint(F("Received from: "));
    debugPrintln(bufDestAddr);
    debugPrint(F("Opcode       : "));
    debugPrintln(*bufOpcode);
    debugPrint(F("The remaining data -> ")); // means, without opcode
    printPayloadBytes(buf_NS_to_Node, RXPayloadL - 1);
    debugPrintln();
}

void packet_is_OK(uint16_t NetworkID, uint8_t PayloadCRC)
{
    debugPrint(F("LocalNetworkID,0x"));
    debugPrintHex(NetworkID);
    debugPrint(F(",TransmittedPayloadCRC,0x")); // print CRC of transmitted packet
    debugPrintHex(PayloadCRC);
    debugPrintln(); // Newline at the end
}

void packet_is_Error(SX128XLT &LT)
{
    debugPrint(F("No Packet acknowledge"));
    LT.printIrqStatus();      // prints the text of which IRQs set
    LT.printReliableStatus(); // print the reliable status
}

void LoRa_to_NS(MQTTPubSubClient &mqtt, uint16_t source_addr, uint8_t opcode, uint8_t *payload_buff, uint8_t payload_buff_len)
{
    String data_forward;

    switch (opcode)
    {
    case OP_CFG_CHK:
        data_forward = formatDataAsString((uint8_t *)&source_addr, &opcode, payload_buff, payload_buff_len);
        fl_connect = mqtt.publish(fullReqTopic, data_forward);
        break;
    case OP_TASK_CHK:
        data_forward = formatDataAsString((uint8_t *)&source_addr, &opcode, payload_buff, payload_buff_len);
        fl_connect = mqtt.publish(fullReqTopic, data_forward);
        break;
    case OP_RANG_MULTI:
        // debugPrint(F("I will perform ranging"));
        break;
    case OP_RANG_RES_PtP:
        data_forward = formatDataAsString((uint8_t *)&source_addr, &opcode, payload_buff, payload_buff_len);
        fl_connect = mqtt.publish(fullResultTopic, data_forward);
        break;
    case OP_RANG_RES_AR:
        data_forward = formatDataAsString((uint8_t *)&source_addr, &opcode, payload_buff, payload_buff_len);
        fl_connect = mqtt.publish(fullResultTopic, data_forward);
        break;
    case OP_DUMMY_LED:
        // data_forward = formatDataAsString(source_addr, opcode, payload_buff, 1); // len of payload buff is 1 byte (for on/off)
        break;
    default:
        debugPrint(F("The instruction is unknown"));
        break;
    }
}

void Packet_to_LoRa(SX128XLT &LT, uint16_t Destination_addr, uint16_t Source_addr, uint8_t *data_buff, uint8_t data_buff_len)
{
    uint16_t PayloadCRC;
    uint8_t TXPacketL;
    // keep transmitting the packet until an ACK is received
    uint8_t attempts = TXattempts;
    do
    {
        TXPacketL = LT.transmitReliableAutoACK_addr(data_buff, data_buff_len, NetworkID, Destination_addr, Source_addr, ACKtimeout, TXtimeout, static_cast<int8_t>(TXpower_set), WAIT_TX);
        attempts--;
        if (TXPacketL > 0)
        {
            PayloadCRC = LT.getTXPayloadCRC(TXPacketL); // read the actual transmitted CRC from the LoRa device buffer
            debugPrintln("Sent to end node");
            break;
            // packet is OK
        }
        debugPrintln(attempts);
    } while ((TXPacketL == 0) && (attempts != 0));
}

void openReceiveWindow()
{
    xSemaphoreTake(LoRa_module_accessed_sem, portMAX_DELAY);
    PacketOK = LT.receiveReliableAutoACK_addr(RXBUFFER, RXBUFFER_SIZE, NetworkID, Node_address, ACKdelay, static_cast<int8_t>(TXpower_set), 0, 0, &bufDestAddr); // Listen until receive packet
    xSemaphoreGive(LoRa_module_accessed_sem);
}