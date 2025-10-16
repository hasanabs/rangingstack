#ifndef CTL_TAG_COMMUN_H
#define CTL_TAG_COMMUN_H
#include <Arduino.h>
#include "ctl_tag_struct.h"
#include "vektor.h"
#include "Settings.h"
#include <SX128XLT.h>
extern SX128XLT LT;

// Payload variables helper
extern uint8_t PacketOK;            // set to > 0 if packetOK
extern const uint8_t RXBUFFER_SIZE; // RX buffer size, set to max payload length of 247, or maximum expected length
extern uint8_t RXBUFFER[];          // create the buffer that received packets are copied into
extern uint16_t RXaddrSource;       // For extracting source address
extern uint8_t RXopcode;            // buffer for opcode

// End node config
extern uint8_t mode;
extern uint16_t msg_check_interv;
extern uint8_t repeat_rang;
extern bool sleepStatus;

extern void printPayload(const DynamicArray &payload_buff);
extern void printPayloadBytes(uint8_t *payload_buff, uint8_t payload_size);
extern void packet_is_OK(uint16_t NetworkID, uint8_t PayloadCRC);
extern void packet_is_Error(SX128XLT &LT);

extern bool createSendPacket(SX128XLT &LT, uint16_t Destination_addr, uint16_t Source_addr, uint8_t opcode, uint32_t tx_timeout, const void *operand = nullptr, bool withACK = false, uint8_t attempts = TXattempts);
extern void rangingIns(uint8_t *payload_buff);
extern void opcode_executor(uint8_t *payload_buff);

#endif // CTL_TAG_COMMUN_H