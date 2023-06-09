/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 06/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

// These are the pin definitions for one of my own boards, the Easy Pro Mini,
// be sure to change the definitions to match your own setup. Some pins such as DIO2,
// DIO3, BUZZER may not be in used by this sketch so they do not need to be
// connected and should be included and be set to -1.

#define NSS 10
#define RFBUSY 7
#define NRESET 9
#define TXLED 8
#define RXLED 6
#define DIO1 3
#define DIO2 -1                   // not used
#define DIO3 -1                   // not used
#define RX_EN -1                  // pin for RX enable, used on some SX1280 devices, set to -1 if not used
#define TX_EN -1                  // pin for TX enable, used on some SX1280 devices, set to -1 if not used
#define BUZZER -1                 // pin for BUZZER, set to -1 if not used
#define BUZZER 5                  // pin for BUZZER, set to -1 if not used
#define LORA_DEVICE DEVICE_SX1280 // we need to define the device we are using

// LoRa Modem Parameters
const uint32_t frequency = 2445000000;     // frequency of transmissions
const int32_t offset = 0;                  // offset frequency for calibration purposes
const uint8_t spreading_factor = LORA_SF7; // LoRa spreading factor
const uint8_t bandwidth = LORA_BW_0400;    // LoRa bandwidth
const uint8_t code_rate = LORA_CR_4_5;     // LoRa coding rate

const int8_t tx_power = 10; // Power for transmissions in dBm

#define RXBUFFER_SIZE 511 // RX buffer size
