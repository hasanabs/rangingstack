#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>

constexpr uint16_t Node_address = 0x0001;
constexpr uint16_t NetworkID = 0x3210; // NetworkID identifies this connection, needs to match value in receiver
constexpr uint16_t GW_Address = 0xFFFF;

// SPI setting
#define NSS 11 // select pin on LoRa device
#define SCK 12
#define MISO 14
#define MOSI 13

// Pins of the controls and flags
#define NRESET 19                 // reset pin on LoRa device
#define RFBUSY 17                 // busy pin on LoRa device
#define DIO1 16                   // DIO1 pin on LoRa device, used for sensing RX and TX done
#define DIO2 15                   // DIO2 pin on LoRa device, used for sensing RX and TX done
#define LORA_DEVICE DEVICE_SX1280 // we need to define the device we are using

// Parameters of LoRa commun
#define TXpower_set 30 // LoRa transmit power from -18 to 13 dBm (or from 0 to 31 integer)
#define ACKdelay 2     // delay in mS before sending acknowledge
#define ACKtimeout 100 // Acknowledge timeout in mS
#define TXtimeout 500  // transmit timeout in mS. If 0 return from transmit function after send.
#define TXattempts 3   // number of times to attempt to TX before failing (default value), can be change in the argument of the function
#define RXtimeout 750  // receive timeout in mS.
#define communFreq CH_freq[10]
#define communSF LORA_SF10
#define communBW LORA_BW_1600
#define communCR LORA_CR_4_5
#define delaySendResultMax 30 // upper bound for delaying sending result of AR to the GW to avoid collision (in seconds)

// Parameter of LoRa ranging
#define distance_adjustment 1.00
#define rangFreq CH_freq[3]
#define rangSF LORA_SF8
#define rangBW LORA_BW_1600
#define rangCR LORA_CR_4_5

#endif