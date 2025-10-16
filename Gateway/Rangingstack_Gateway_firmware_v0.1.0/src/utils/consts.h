#ifndef CONSTS_H
#define CONSTS_H
#include <Arduino.h>

// Instruction sets code (Opcodes), passing through communication.
enum Opcode : uint8_t
{
    OP_CFG_CHK = 0x00,       // For node to NS, to check the config parameters
    OP_CFG_RES = 0x01,       // For NS to node, to response opcode for receiving config (given by GW)
    OP_TASK_CHK = 0x02,      // For node to NS, to check if there any instruction for it
    OP_RANG_MULTI = 0x03,    // For NS to node, to give instruction end node (for PtP ranging)
    OP_RANG_REQ = 0x04,      // For node to node, to denote permision to do ranging
    OP_RANG_RES_PtP = 0x05,  // For node to GW, to denote the result of PtP ranging
    OP_RANG_MULTI_AR = 0x06, // For NS to node, to give instruction end node (for passive ranging)
    OP_RANG_RES_AR = 0x07,   // For node to GW, to denote the result of Passive ranging
    OP_DUMMY_LED = 0x99,     // For GW to node, to turn on/off the node's led
    OP_NULL = 0xFF           // For NS to GW, indicating that no task for a node (MQTT only)
};

#endif // CONSTS_H