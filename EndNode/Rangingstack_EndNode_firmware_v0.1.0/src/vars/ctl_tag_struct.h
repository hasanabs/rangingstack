#ifndef CTL_TAG_STRUCT_H
#define CTL_TAG_STRUCT_H
#include <Arduino.h>

// Instruction sets code (Opcodes)
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

enum RangRole : uint8_t
{
    ROLE_MASTER = 0x01,
    ROLE_SLAVE = 0x02
};

enum TagMode : uint8_t
{
    MODE_LP = 0x01,       // Low power mode
    MODE_AON_SLAVE = 0x02 // Always on as the slave for ranging mode
};

enum StateLed : uint8_t
{
    STATE_OFF = 0x00,
    STATE_ON = 0x01
};

#pragma pack(push, 1) // Ensure no padding is added between members (with padding, a struct size is multiple of CPU's bit)

struct ranging_block_struct // PtP ranging
{
    uint8_t Role; // master or slave (0x01 or 0x02)
    uint16_t Destination_addr;
    uint16_t Remaining_time;
}; // size 5 bytes

struct ranging_ptp_result
{
    uint16_t Ranging_id;
    uint16_t Slave_addr;
    uint16_t FEI_corr_val;
    uint8_t RRSSI;
}; // size 7 bytes

struct AR_block_struct // passive ranging
{
    uint16_t master_id;
    uint16_t slave_id;
    uint16_t Remaining_time;
}; // size 6 bytes

struct ranging_passive_result
{
    uint16_t Ranging_id;
    uint16_t master_id;
    uint16_t slave_id;
    uint16_t raw_val;
    uint8_t RRSSI; // relative to slave
}; // size 9 bytes
#pragma pack(pop)

#endif // CTL_TAG_STRUCT_H