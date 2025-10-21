#include <Arduino.h>
#include <SPI.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <timers.h>
#include "settings.h"
#include "tasks.h"
#include "ctl_tag_commun.h"
#include "stat.h"
#include "batStatus.h"
#include "fun_led.h"

// #define debugRanging

SemaphoreHandle_t xBlock_AOn_Rang_sem;
SemaphoreHandle_t xBlock_LP_Commun_sem;

TaskHandle_t TaskMsgCheck_Handle = NULL;
TaskHandle_t TaskMsgCheck_AOn_Handle = NULL;
TaskHandle_t TaskListen_Handle = NULL;
TimerHandle_t TaskListen_TimerHandle = NULL;
TaskHandle_t TaskRangingSlave_AOn_Handle = NULL;

uint16_t ranging_id;          // helper variable to give the same random ID for each batch of ranging
uint8_t trialToGetConfig = 3; // Number of attempt of try for geting config (when config response received by gateway)

uint16_t generateRandom16()
{
    return static_cast<uint16_t>(random(0, 65536));
}

void preSleep()
{ // before vTaskDelay
    if (!sleepStatus)
    {
        LT.setSleep(CONFIGURATION_RETENTION); // put SX1280 into sleep mode with RAM retention
        SPI.end();
        sleepStatus = true;
    }
}

void afterWake()
{ // after vTaskDelay
    if (sleepStatus)
    {
        SPI.begin();
        LT.wake();
        sleepStatus = false;
    }
}

void TaskCfgCheck(void *pvParameters)
{
    createSendPacket(LT, 0xFFFF, Node_address, OP_CFG_CHK, 1000); // Send the GW MSG check
    xTaskCreate(TaskListen, "Listen NS's Init", 2048, NULL, 4, &TaskListen_Handle);
    xTimerStart(TaskListen_TimerHandle, 0);
    preSleep();
    delayMicroseconds(500);
    vTaskDelay(4);                                          // give some amount of time so that if fl_cfg_sent is true, there is a chance to listen the response from NS
    if (RXBUFFER[0] != OP_CFG_RES && trialToGetConfig != 0) // in case data sent but no config response
    {
        trialToGetConfig -= 1;
        afterWake();
        delay(1);
        xTaskCreate(TaskCfgCheck, "Cfg Check", 2048, NULL, 4, NULL);
        vTaskDelete(NULL);
        return;
    }
    else if (RXBUFFER[0] != OP_CFG_RES && trialToGetConfig == 0)
    { // if no config instruction & trial is over, take Low power mode as default
        xTaskCreate(TaskMsgCheck, "Msg Check", 2048, NULL, 4, &TaskMsgCheck_Handle);
    }
    vTaskDelete(NULL);
}

void TaskMsgCheck(void *pvParameters)
{
    (void)pvParameters;
    vTaskDelay(1);
    while (1)
    {
        afterWake();
        delayMicroseconds(500);
        bat_level = read_bat_status();
        createSendPacket(LT, GW_Address, Node_address, OP_TASK_CHK, TXtimeout, &bat_level, true); // Send the GW MSG check
        xTaskCreate(TaskListen, "Listen NS's Resp", 2048, NULL, 4, &TaskListen_Handle);
        xTimerStart(TaskListen_TimerHandle, 0);
        preSleep();
        delayMicroseconds(500);
        vTaskDelay((TickType_t)msg_check_interv);
    }
}

void TaskListen_CallbackTimer(TimerHandle_t xTimer)
{
    xTaskNotifyGive(TaskListen_Handle); // Notify Task responder
}

void TaskListen(void *pvParameters)
{
    // Wait for notification from the timer callback
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    afterWake();
    delayMicroseconds(500);
    memset(RXBUFFER, 0, RXBUFFER_SIZE);
    xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
    bool preambleDetected = LT.setCad(12); // see if there is preamble or not (22 ms>21.76 for 8 symbol of SF12 BW 1600)
    if (preambleDetected)                  // open receive window only if there is preamble detected
    {
        PacketOK = LT.receiveReliableAutoACK_addr(RXBUFFER, RXBUFFER_SIZE, NetworkID, Node_address, ACKdelay, static_cast<int8_t>(TXpower_set), RXtimeout, WAIT_RX, &RXaddrSource);
        if (PacketOK > 0)
        {
            if (mode != MODE_AON_SLAVE)
            {
                opcode_executor(RXBUFFER);
                delay(1);
            }
            else
            { // in AOn mode, meaningful instruction is only change mode (as AOn slave devoted as ranging slave)
                if (*RXBUFFER == OP_CFG_RES)
                {
                    opcode_executor(RXBUFFER);
                    delay(1);
                }
            }
        }
    }
    xSemaphoreGive(xBlock_LP_Commun_sem);
    preSleep();
    delayMicroseconds(500);
    vTaskDelete(NULL);
}

void TaskMsgCheck_AOn(void *pvParameters)
{
    (void)pvParameters;
    afterWake();
    delay(5);
    vTaskDelay(1); // give time to deletetation other task stuff at the initialization
    while (1)
    {
        xSemaphoreTake(xBlock_AOn_Rang_sem, portMAX_DELAY);
        bat_level = read_bat_status();
        createSendPacket(LT, GW_Address, Node_address, OP_TASK_CHK, TXtimeout, &bat_level, true); // Send the GW MSG check
        xTaskCreate(TaskListen_AOn, "Listen NS's Resp", 2048, NULL, 4, NULL);
        vTaskDelay((TickType_t)msg_check_interv);
    }
}

void TaskListen_AOn(void *pvParameters)
{
    vTaskDelay(3);
    delay(1);
    memset(RXBUFFER, 0, RXBUFFER_SIZE);
    PacketOK = LT.receiveReliableAutoACK_addr(RXBUFFER, RXBUFFER_SIZE, NetworkID, Node_address, ACKdelay, static_cast<int8_t>(TXpower_set), RXtimeout, WAIT_RX, &RXaddrSource); // wait 300 ms timeout
    if (PacketOK > 0)
    {
        // in AOn mode, meaningful instruction is only change mode (as AOn slave devoted as ranging slave)
        if (*RXBUFFER == OP_CFG_RES)
        {
            opcode_executor(RXBUFFER);
            delay(1);
        }
    }
    if (mode == MODE_AON_SLAVE) // when receiving opcode to change mode, this will not be executed (no ranging slave AOn routine)
    {
        if (TaskRangingSlave_AOn_Handle == NULL)
        {
            xTaskCreate(TaskRangingSlave_AOn, "Ranging Slave AOn", 4096, NULL, 3, &TaskRangingSlave_AOn_Handle);
        }
    }
    xSemaphoreGive(xBlock_AOn_Rang_sem);
    vTaskDelete(NULL);
}

void TaskRanging(void *pvParameters)
{
    (void)pvParameters;
    // bool fl_ranging_sucess = false;
    ranging_block_struct ranging_identifier{0, 0, 0};
    std::memcpy(&ranging_identifier, pvParameters, sizeof(ranging_block_struct));
    ranging_id = generateRandom16();
    preSleep();
    delayMicroseconds(500);
    if (ranging_identifier.Role == ROLE_MASTER)
    {
        vTaskDelay(ranging_identifier.Remaining_time);
    }
    else
    {
        vTaskDelay(ranging_identifier.Remaining_time - 1);
    }
    afterWake();
    delay(1);
    if (ranging_identifier.Role == ROLE_MASTER)
    {
        rangingMaster(ranging_identifier.Destination_addr);
    }
    else if (ranging_identifier.Role == ROLE_SLAVE)
    {
        rangingSlave();
    }
    preSleep();
    delay(5);
    vTaskDelete(NULL);
}

void TaskAR(void *pvParameters)
{
    (void)pvParameters;
    AR_block_struct ranging_identifier{0, 0, 0};
    std::memcpy(&ranging_identifier, pvParameters, sizeof(AR_block_struct));
    ranging_id = generateRandom16();
    preSleep();
    delayMicroseconds(500);
    vTaskDelay(ranging_identifier.Remaining_time - 1); // wake up 1 second earlier
    afterWake();
    delay(1);
    rangingPassive(ranging_identifier.master_id, ranging_identifier.slave_id);
    preSleep();
    delay(5);
    vTaskDelete(NULL);
}

void rangingMaster(uint16_t Destination_address)
{
    int32_t range_result;
    float distance;
    float RSSI_buff;

    uint8_t sucess_count = 0;
    uint8_t ranging_count = repeat_rang;
    bool fl_sent = createSendPacket(LT, Destination_address, Node_address, OP_RANG_REQ, 400, &repeat_rang, true, 3);
    if (fl_sent)
    {
        ranging_ptp_result resultRanging = {ranging_id, Destination_address, 0, 0};
        xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
        LT.setupRanging(rangFreq, 0, rangSF, rangBW, rangCR, Destination_address, RANGING_MASTER);
        delay(5); // give time the slave to be ready
        Vektor<float> dist_arr;
        RSSI_buff = 0;
        for (int i = 0; i < ranging_count; i++)
        {
            LT.transmitRanging(Destination_address, 100, TXpower_set, WAIT_TX);
            range_result = LT.getRangingResultRegValue(RANGING_RESULT_RAW);
            distance = LT.getRangingDistance(RANGING_RESULT_RAW, range_result, distance_adjustment);
            if (distance > 0)
            {
                sucess_count++;
                dist_arr.push_back(distance - LT.getFrequencyErrorHz() * LT.lookupFeiFactor(rangSF, rangBW) * 1e-3); // FEI corrected
                RSSI_buff += LT.getRangingRSSI();                                                                    // For positive value (to make for uint8_t suit)
            }
        }
        if (sucess_count > 0)
        {
            quickSort(dist_arr, 0, (int)dist_arr.getSize() - 1);
            resultRanging.FEI_corr_val = (uint16_t)10 * (calculateMedian(dist_arr, 0, dist_arr.getSize() - 1)); // scale up 10x to enable a better resolution with only 2 byte data (expected original range within 10 to 2000)
            resultRanging.RRSSI = (uint8_t)(RSSI_buff / sucess_count);
        }
        LT.setupLoRa(communFreq, 0, communSF, communBW, communCR);
        delay(5);
        xSemaphoreGive(xBlock_LP_Commun_sem);
        if (sucess_count > 0)
        {
            createSendPacket(LT, GW_Address, Node_address, OP_RANG_RES_PtP, 500, &resultRanging, true, 3); // Report num of success ranging to GW
        }
    }
}

void rangingSlave()
{
    bool fl_rang_sucess = false;
    xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
    PacketOK = LT.receiveReliableAutoACK_addr(RXBUFFER, RXBUFFER_SIZE, NetworkID, Node_address, ACKdelay, static_cast<int8_t>(TXpower_set), 2000, WAIT_RX, &RXaddrSource);
    xSemaphoreGive(xBlock_LP_Commun_sem);
    if (PacketOK)
    {
        RXopcode = *RXBUFFER;
        if (RXopcode == OP_RANG_REQ)
        {
            uint8_t ranging_times = *(RXBUFFER + 1); // The second byte of buffer is the number of repetation of ranging
            xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
            LT.setupRanging(rangFreq, 0, rangSF, rangBW, rangCR, Node_address, RANGING_SLAVE);
            uint8_t sucess_count = 0;
            for (int i = 0; i < ranging_times; i++)
            {
                if (i != 0)
                {
                    fl_rang_sucess = LT.receiveRanging(Node_address, 30, TXpower_set, WAIT_RX);
                }
                else // only first iteration
                {
                    fl_rang_sucess = LT.receiveRanging(Node_address, 50, TXpower_set, WAIT_RX);
                }
                if (fl_rang_sucess)
                {
                    sucess_count++;
                }
            }
            LT.setupLoRa(communFreq, 0, communSF, communBW, communCR);
            delay(1);
            xSemaphoreGive(xBlock_LP_Commun_sem);
        }
    }
}

void TaskRangingSlave_AOn(void *pvParameters)
{
    (void)pvParameters;
    bool fl_rang_sucess = false;
    while (1)
    {
        PacketOK = false;
        xSemaphoreTake(xBlock_AOn_Rang_sem, portMAX_DELAY);
        PacketOK = LT.receiveReliableAutoACK_addr(RXBUFFER, RXBUFFER_SIZE, NetworkID, Node_address, ACKdelay, static_cast<int8_t>(TXpower_set), 0, WAIT_RX, &RXaddrSource); // wait until receive a ranging request
        xSemaphoreGive(xBlock_AOn_Rang_sem);
        if (PacketOK)
        {
            RXopcode = *RXBUFFER;
            if (RXopcode == OP_RANG_REQ)
            {
                uint8_t ranging_times = *(RXBUFFER + 1); // The second byte of buffer is the number of repetation of ranging
                xSemaphoreTake(xBlock_AOn_Rang_sem, portMAX_DELAY);
                LT.setupRanging(rangFreq, 0, rangSF, rangBW, rangCR, Node_address, RANGING_SLAVE);
                uint8_t sucess_count = 0;
                for (int i = 0; i < ranging_times; i++)
                {
                    if (i != 0)
                    {
                        fl_rang_sucess = LT.receiveRanging(Node_address, 30, TXpower_set, WAIT_RX);
                    }
                    else // only first iteration
                    {
                        fl_rang_sucess = LT.receiveRanging(Node_address, 50, TXpower_set, WAIT_RX);
                    }
                    if (fl_rang_sucess)
                    {
                        sucess_count++;
                    }
                }
                LT.setupLoRa(communFreq, 0, communSF, communBW, communCR);
                delay(1);
                xSemaphoreGive(xBlock_AOn_Rang_sem);
            }
        }
    }
}

void rangingPassive(uint16_t Master_id, uint16_t Slave_id)
{
    int32_t advancedRanging_result;
    float delta_distance; // (A-to-S) - (A-to-M) => [AR receive the signal of master first, then receive the signal of slave]
    float RRSSI_AR_buff;  // The closer from the slave, the better is the ranging RSSI

    uint8_t ranging_count;
    uint8_t sucess_count = 0;
    xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
    PacketOK = LT.receiveReliable_addr(RXBUFFER, RXBUFFER_SIZE, NetworkID, Slave_id, 0, WAIT_RX, &RXaddrSource);
    xSemaphoreGive(xBlock_LP_Commun_sem);
    if (PacketOK)
    {
        RXopcode = *RXBUFFER;
        if (RXopcode == OP_RANG_REQ)
        {
            ranging_count = *(RXBUFFER + 1); // The second byte of buffer is the number of repetation of ranging
        }
        uint16_t AR_Master_Addr = RXaddrSource; // Extract Master address from the payload
        // As if this node act as AR master node checking the ACK of slave, to make sure that the slave is ready for ranging. So the AR node can be sure that AR can be done soon. Otherwise, no need to open AR window
        uint16_t payloadcrc = LT.readUint16SXBuffer(8);
        xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
        PacketOK = LT.waitReliableACK_addr(NetworkID, Slave_id, AR_Master_Addr, payloadcrc, ACKtimeout);
        xSemaphoreGive(xBlock_LP_Commun_sem);
        if (PacketOK)
        {
            ranging_passive_result resultRanging = {ranging_id, AR_Master_Addr, Slave_id, 0, 0};
            xSemaphoreTake(xBlock_LP_Commun_sem, portMAX_DELAY);
            LT.setupRanging(rangFreq, 0, rangSF, rangBW, rangCR, Node_address, RANGING_ADVANCED);
            sucess_count = 0;
            Vektor<float> delta_dist_arr;
            RRSSI_AR_buff = 0;
            for (int i = 0; i < ranging_count; i++)
            {
                LT.receiveAdvancedRanging(25, WAIT_RX);
                advancedRanging_result = LT.getRangingResultRegValue(RANGING_RESULT_RAW);
                delta_distance = LT.getRangingDistance(RANGING_RESULT_RAW, advancedRanging_result, distance_adjustment);
                if (delta_distance > 0)
                {
                    sucess_count++;
                    delta_dist_arr.push_back(delta_distance);
                    RRSSI_AR_buff += LT.getRangingRSSI();
                }
            }
            if (sucess_count > 0)
            {
                quickSort(delta_dist_arr, 0, (int)delta_dist_arr.getSize() - 1);
                resultRanging.raw_val = (uint16_t)10 * (calculateMedian(delta_dist_arr, 0, delta_dist_arr.getSize() - 1)); // scale up 10x to enable a better resolution with only 2 byte data (expected original range within 10 to 2000)
                resultRanging.RRSSI = (uint8_t)(RRSSI_AR_buff / sucess_count);
            }
            LT.setupLoRa(communFreq, 0, communSF, communBW, communCR);
            delay(5);
            xSemaphoreGive(xBlock_LP_Commun_sem);
            if (sucess_count > 0)
            {
                uint8_t delayTransmission = static_cast<uint8_t>(random(2, delaySendResultMax)); // in seconds unit (because a single context switching in this node set as 1000 ms)
                preSleep();
                delay(1);
                vTaskDelay(delayTransmission); // delay tranmission
                afterWake();
                createSendPacket(LT, GW_Address, Node_address, OP_RANG_RES_AR, 500, &resultRanging, true, 3); // Report num of success ranging to GW
            }
        }
    }
}