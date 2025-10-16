#include "ctl_tag_commun.h"
#include "Settings.h"
#include <Arduino.h>
#include "fun_led.h"
#include "vektor.h"
#include "tasks.h"
#include "ctl_tag_struct.h"
#include <SX128XLT.h>
SX128XLT LT;

// End node config
uint8_t mode = MODE_LP;
uint16_t msg_check_interv = 300;
uint8_t repeat_rang = 10;
uint16_t seed_helper; // for random seed parameter purpose given by NS
bool sleepStatus = false;

// Payload variables helper
uint8_t PacketOK;                  // set to > 0 if packetOK
const uint8_t RXBUFFER_SIZE = 247; // RX buffer size, set to max payload length of 247, or maximum expected length
uint8_t RXBUFFER[RXBUFFER_SIZE];   // create the buffer that received packets are copied into
uint16_t RXaddrSource;             // For extracting source address
uint8_t RXopcode;                  // buffer for opcode

void packet_is_Error(SX128XLT &LT)
{
    // Serial.print(F("No Packet acknowledge"));
    LT.printIrqStatus();      // prints the text of which IRQs set
    LT.printReliableStatus(); // print the reliable status
}

bool createSendPacket(SX128XLT &LT, uint16_t Destination_addr, uint16_t Source_addr, uint8_t opcode, uint32_t tx_timeout, const void *operand, bool withACK, uint8_t attempts) // Destination, source, opcode, operand, & ack_needed_or_no
{
    uint8_t TXPacketL;
    DynamicArray payload_buff;
    payload_buff.push_back(reinterpret_cast<const uint8_t *>(&opcode), sizeof(uint8_t));
    switch (opcode)
    {
    case OP_CFG_CHK:
        break;
    case OP_TASK_CHK:
        payload_buff.push_back(reinterpret_cast<const uint8_t *>(operand), 1); // a byte of operand represents batery status
        break;
    case OP_RANG_RES_PtP:
        payload_buff.push_back(reinterpret_cast<const uint8_t *>(operand), sizeof(ranging_ptp_result));
        break;
    case OP_RANG_RES_AR:
        payload_buff.push_back(reinterpret_cast<const uint8_t *>(operand), sizeof(ranging_passive_result));
        break;
    case OP_RANG_REQ:
        payload_buff.push_back(reinterpret_cast<const uint8_t *>(operand), 1); // a byte of operand represents num of repeatation of ranging
        break;
    case OP_DUMMY_LED:
        // restart operand
        break;
    default:
        // nothing
        break;
    }
    // keep transmitting the packet until an ACK is received
    uint8_t Tx_attempts = attempts;
    uint8_t delayRetransmission = 0;
    do
    {
        xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
        if (withACK)
        {
            TXPacketL = LT.transmitReliableAutoACK_addr(payload_buff.getData(), static_cast<uint8_t>(payload_buff.getSize()), NetworkID, Destination_addr, Source_addr, ACKtimeout, tx_timeout, static_cast<int8_t>(TXpower_set), WAIT_TX);
        }
        else
        {
            TXPacketL = LT.transmitReliable_addr(payload_buff.getData(), static_cast<uint8_t>(payload_buff.getSize()), NetworkID, Destination_addr, Source_addr, tx_timeout, static_cast<int8_t>(TXpower_set), WAIT_TX);
        }
        xSemaphoreGive(xBlock_LP_Commun_sem);
        Tx_attempts--;
        if (TXPacketL > 0)
        {
            return true;
        }
        if ((Tx_attempts != 0))
        {
            delay(random(0, 3) * 100);                                                 // add grid of 100 ms delay so that chance of collision become smaller (assuming time on air < 100 ms for the packet)
            delayRetransmission = static_cast<uint8_t>(random(2, delaySendResultMax)); // in seconds unit (because a single context switching in this node set as 1000 ms)
            preSleep();
            delay(1);
            vTaskDelay(delayRetransmission); // delay between tranmission attampts
            afterWake();
            delay(1);
        }
    } while ((TXPacketL == 0) && (Tx_attempts != 0));
    return false;
}

void rangingIns(uint8_t *payload_buff)
{
    uint8_t num_of_task = *payload_buff; // The first byte of the payload is the information about the batch task number
    for (int i = 0; i < num_of_task; i++)
    {
        char taskName[16];
        snprintf(taskName, sizeof(taskName), "Rang_%d", i);
        xTaskCreate(TaskRanging, taskName, 1024, payload_buff + 1 + i * sizeof(ranging_block_struct), 3, NULL);
    }
}

void passiveRangingIns(uint8_t *payload_buff)
{
    uint8_t num_of_task = *payload_buff; // The first byte of the payload is the information about the batch task number
    for (int i = 0; i < num_of_task; i++)
    {
        char taskName[16];
        snprintf(taskName, sizeof(taskName), "AR_%d", i);
        xTaskCreate(TaskAR, taskName, 1024, payload_buff + 1 + i * sizeof(AR_block_struct), 3, NULL);
    }
}

void opcode_executor(uint8_t *payload_buff)
{
    uint8_t opcode = *payload_buff;
    uint8_t *operand = payload_buff + sizeof(uint8_t); // to point data after the opcode
    switch (opcode)
    {
    case OP_CFG_RES:
        mode = *(operand);
        msg_check_interv = (static_cast<uint16_t>(operand[2]) << 8) | operand[1];
        repeat_rang = *(operand + 3);
        if (mode != MODE_AON_SLAVE)
        {
            if (TaskMsgCheck_AOn_Handle != NULL)
            { // if routine MsgCheck_AOn exist, delete this routine first
                vTaskDelete(TaskMsgCheck_AOn_Handle);
                TaskMsgCheck_AOn_Handle = NULL;
            }
            if (TaskRangingSlave_AOn_Handle != NULL)
            { // if routine RangingSlave_AOn exist, delete this routine first
                vTaskDelete(TaskRangingSlave_AOn_Handle);
            }
            if (TaskMsgCheck_Handle == NULL)
            { // Create the task if it doesn't exist
                xTaskCreate(TaskMsgCheck, "Msg Check", 1024, NULL, 4, &TaskMsgCheck_Handle);
            }
        }
        else
        {
            if (TaskMsgCheck_Handle != NULL)
            { // if routine MsgCheck exist, delete this routine first
                vTaskDelete(TaskMsgCheck_Handle);
                TaskMsgCheck_Handle = NULL;
            }
            if (TaskMsgCheck_AOn_Handle == NULL)
            { // Create the task if it doesn't exist
                xTaskCreate(TaskMsgCheck_AOn, "Msg Check AOn", 1024, NULL, 4, &TaskMsgCheck_AOn_Handle);
            }
        }
        break;
    case OP_RANG_MULTI:
        rangingIns(operand);
        break;
    case OP_RANG_MULTI_AR:
        passiveRangingIns(operand);
        break;
    case OP_DUMMY_LED:
        switch (*operand)
        {
        case 1:
            led_On();
            break;
        default:
            led_Off();
        }
        break;
    default:
        delay(0); // do nothing
    }
}
