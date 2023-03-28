#define Program_Version "V1.0"

#include <SPI.h>
#include "mySX1280.h"
#include "Settings.h"

uint8_t buff[] = "Hello World 1234567890"; // TODO: What is this used for?

mySX1280::mySX1280() {}

void mySX1280::Setup()
{
  pinMode(LED1, OUTPUT);
  LedFlash(2, 125);

  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("LoRa Starting"));
  Serial.println();

  SPI.begin();

  if (lora.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, RX_EN, TX_EN, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    LedFlash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      LedFlash(50, 50); // long fast speed LED flash indicates device error
    }
  }

  lora.setupLoRa(frequency, offset, spreading_factor, bandwidth, code_rate);
  // lora.setLowPowerRX();

  Serial.println();
  lora.printModemSettings();
  Serial.println();
  // lora.printOperatingSettings();
  // Serial.println();
  // Serial.println();
  // lora.printRegisters(0x900, 0x9FF);
  // Serial.println();
  // Serial.println();

  // Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  // Serial.println(RXBUFFER_SIZE);
  // Serial.println();
}

void mySX1280::UpdateSettings(uint8_t new_spreading_factor, uint8_t new_bandwidth, uint8_t new_code_rate)
{
  lora.setupLoRa(frequency, offset, new_spreading_factor, new_bandwidth, new_code_rate);
  Serial.println();
  lora.printModemSettings();
  Serial.println();
}

void mySX1280::Transmit(uint8_t message[], uint16_t message_size)
{
  Serial.print(tx_power);
  Serial.print(F("dBm "));
  Serial.print(F("Packet> "));
  Serial.flush();

  tx_packet_length = message_size;
  message[tx_packet_length - 1] = '*'; // replace null character at buffer end so its visible on reciver

  lora.printASCIIPacket(message, tx_packet_length);

  digitalWrite(LED1, HIGH);
  start_ms = millis();
  if (lora.transmit(message, tx_packet_length, 10000, tx_power, WAIT_TX)) // will return packet length sent if OK, otherwise 0 if transmit, timeout 10 seconds
  {
    end_ms = millis(); // packet sent, note end time
    tx_packet_count++;
    TransmitIsOK();
  }
  else
  {
    TransmitIsError(); // transmit packet returned 0, there was an error
  }

  digitalWrite(LED1, LOW);
  Serial.println();
}

void mySX1280::TransmitIsError()
{
  uint16_t irq_status;
  irq_status = lora.readIrqStatus();
  Serial.print(F(" SendError,"));
  Serial.print(F("Length,"));
  Serial.print(tx_packet_length);
  Serial.print(F(",IRQreg,"));
  Serial.print(irq_status, HEX);
  lora.printIrqStatus();
}

void mySX1280::TransmitIsOK()
{
  uint16_t loral_crc;

  Serial.print(F("  BytesSent,"));
  Serial.print(tx_packet_length);
  loral_crc = lora.CRCCCITT(buff, tx_packet_length, 0xFFFF);
  Serial.print(F("  CRC,"));
  Serial.print(loral_crc, HEX);
  Serial.print(F("  TransmitTime,"));
  Serial.print(end_ms - start_ms);
  Serial.print(F("mS"));
  Serial.print(F("  PacketsSent,"));
  Serial.print(tx_packet_count);
}

void mySX1280::Receive()
{
  rx_packet_length = lora.receive(rx_buffer, RXBUFFER_SIZE, 200, 1); // wait for a packet to arrive with 60seconds (60000mS) timeout

  digitalWrite(LED1, HIGH);

  packet_rssi = lora.readPacketRSSI();
  packet_snr = lora.readPacketSNR();

  if (rx_packet_length == 0) // if the lora.receive() function detects an error, rx_packet_length == 0
  {
    ReceivePacketIsError();
  }
  else
  {
    ReceivePacketIsOK();
  }

  digitalWrite(LED1, LOW);
}

void mySX1280::ReceivePacketIsOK()
{
  uint16_t irq_status, local_crc;

  irq_status = lora.readIrqStatus();

  rx_packet_count++;

  PrintElapsedTime();
  Serial.print(F("  "));
  lora.printASCIIPacket(rx_buffer, rx_packet_length);

  local_crc = lora.CRCCCITT(rx_buffer, rx_packet_length, 0xFFFF); // calculate the CRC, this is the external CRC calculation of the rx_buffer
  Serial.print(F(",CRC,"));                                       // contents, not the LoRa device internal CRC
  Serial.print(local_crc, HEX);
  Serial.print(F(",RSSI,"));
  Serial.print(packet_rssi);
  Serial.print(F("dBm,SNR,"));
  Serial.print(packet_snr);
  Serial.print(F("dB,Length,"));
  Serial.print(rx_packet_length);
  Serial.print(F(",Packets,"));
  Serial.print(rx_packet_count);
  Serial.print(F(",Errors,"));
  Serial.print(error_count);
  Serial.print(F(",IRQreg,"));
  Serial.print(irq_status, HEX);
  Serial.println();
}

void mySX1280::ReceivePacketIsError()
{
  uint16_t irq_status;
  irq_status = lora.readIrqStatus();

  PrintElapsedTime();

  if (irq_status & IRQ_RX_TIMEOUT) // check for an RX timeout
  {
    Serial.print(F(" RXTimeout"));
  }
  else
  {
    error_count++;
    Serial.print(F(" PacketError"));
    Serial.print(F(",RSSI,"));
    Serial.print(packet_rssi);
    Serial.print(F("dBm,SNR,"));
    Serial.print(packet_snr);
    Serial.print(F("dB,Length,"));
    Serial.print(lora.readRXPacketL()); // get the real packet length
    Serial.print(F(",Packets,"));
    Serial.print(rx_packet_count);
    Serial.print(F(",Errors,"));
    Serial.print(error_count);
    Serial.print(F(",IRQreg,"));
    Serial.print(irq_status, HEX);
    lora.printIrqStatus(); // print the names of the IRQ registers set
  }

  // delay(250);
}

void mySX1280::LedFlash(uint8_t flashes, uint16_t delay_ms)
{
  for (uint8_t i = 0; i < flashes; i++)
  {
    digitalWrite(LED1, HIGH);
    delay(delay_ms);
    digitalWrite(LED1, LOW);
    delay(delay_ms);
  }
}

void mySX1280::PrintElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}