#ifndef LORA_DRIVER_H
#define LORA_DRIVER_H
#include <Arduino.h>
#include <SX128XLT.h>
#include <MQTTPubSubClient.h>

extern SX128XLT LT; // create a library class instance called LT
extern SemaphoreHandle_t LoRa_module_accessed_sem;

// LoRa Receive variable helper
constexpr uint8_t RXBUFFER_SIZE = 247; // RX buffer size, set to max payload length of 247, or maximum expected length
extern uint8_t RXBUFFER[];             // create the buffer that received packets are copied into
extern uint8_t RXPacketL;              // stores length of packet received
extern uint8_t RXPayloadL;             // stores length of payload received
extern uint8_t PacketOK;               // set to > 0 if packetOK
extern uint16_t bufDestAddr;           // A temp variable for putting node's address

/**
 * @brief For initializing SX1280 functionalities.
 */
extern void LoRa_init();

/**
 * @brief Prints a buffer of payload bytes in hexadecimal for debugging.
 *
 * @param payload_buff Pointer to the payload data.
 * @param payload_size Number of bytes to print.
 */
extern void printPayloadBytes(uint8_t *payload_buff, uint8_t payload_size);

/**
 * @brief Debug printer for LoRa RX metadata.
 *
 * Prints the source address, opcode, and the remaining payload bytes.
 *
 * @param bufDestAddr   Address of the received packet.
 * @param bufOpcode     Pointer to the opcode byte.
 * @param buf_NS_to_Node Pointer to the start of the payload bytes.
 */
extern void debugerPrinter(uint16_t bufDestAddr, uint8_t *bufOpcode, uint8_t *buf_NS_to_Node);

/**
 * @brief Logs a successful LoRa transmission CRC.
 *
 * @param NetworkID    Local network ID.
 * @param PayloadCRC   CRC value of the transmitted payload.
 */
extern void packet_is_OK(uint16_t NetworkID, uint8_t PayloadCRC);

/**
 * @brief Logs a failed LoRa transmission with IRQ status.
 *
 * @param LT Reference to the SX128XLT instance to query status from.
 */
extern void packet_is_Error(SX128XLT &LT);

/**
 * @brief Publishes a LoRa→NS packet over MQTT using the correct topic per opcode.
 *
 * @param mqtt           MQTT client reference.
 * @param source_addr    Node address sending the data.
 * @param opcode         Operation code determining the action.
 * @param payload_buff   Pointer to payload data.
 * @param payload_buff_len Payload length (after opcode).
 */
extern void LoRa_to_NS(MQTTPubSubClient &mqtt, uint16_t source_addr, uint8_t opcode, uint8_t *payload_buff, uint8_t payload_buff_len);

/**
 * @brief Sends a reliable LoRa packet with auto-ACK and retries.
 *
 * Retransmits until an ACK is received or attempts run out, printing status to debug.
 *
 * @param LT             Reference to the SX128XLT instance.
 * @param Destination_addr Node address to send to.
 * @param Source_addr    Gateway’s source address.
 * @param data_buff      Pointer to the data buffer.
 * @param data_buff_len  Number of data bytes.
 */
extern void Packet_to_LoRa(SX128XLT &LT, uint16_t Destination_addr, uint16_t Source_addr, uint8_t *data_buff, uint8_t data_buff_len);

/**
 * @brief Opens the LoRa receive window.
 *
 * Takes the LoRa module semaphore, calls SX128XLT::receiveReliableAutoACK_addr to block until
 * a packet arrives (with auto‐ACK), then releases the semaphore.
 */
extern void openReceiveWindow();

#endif // LORA_DRIVER_H