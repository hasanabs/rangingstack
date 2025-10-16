#include <Arduino.h>

// Debug by Print
#define DEBUG_MODE true

// SPI setting
#define NSS 8   // select pin on LoRa device
#define SCK 36  // SCK on SPI3
#define MISO 37 // MISO on SPI3
#define MOSI 35 // MOSI on SPI3

// Pins of the controls and flags
#define NRESET 18                 // reset pin on LoRa device
#define RFBUSY 7                  // busy pin on LoRa device
#define DIO1 17                   // DIO1 pin on LoRa device, used for sensing RX and TX done
#define DIO2 6                    // DIO2 pin on LoRa device, used for sensing RX and TX done
#define LORA_DEVICE DEVICE_SX1280 // we need to define the device we are using

// Parameters of LoRa commun
#define TXpower_set 30   // LoRa transmit power from -18 to 13 dBm (or from 0 to 31 integer)
#define ACKdelay 2       // delay in mS before sending acknowledge
#define ACKtimeout 100   // Acknowledge timeout in mS
#define TXtimeout 250    // transmit timeout in mS. If 0 return from transmit function after send.
#define TXattempts 3     // number of times to attempt to TX and get an Ack before failing
#define RXtimeout 60000  // receive timeout in mS.
#define ListenDelay 3000 // Delay in listening response from the GW (in ms)
#define communFreq CH_freq[10]
#define communSF LORA_SF10
#define communBW LORA_BW_1600
#define communCR LORA_CR_4_5