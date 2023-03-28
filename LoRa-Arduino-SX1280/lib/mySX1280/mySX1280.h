#ifndef mySX1280_h
#define mySX1280_h

#include <Arduino.h>
#include <SX128XLT.h>


class mySX1280
{
public:
  mySX1280();

  void Setup();
  void UpdateSettings(uint8_t new_spreading_factor, uint8_t new_bandwidth, uint8_t new_code_rate);
  void Transmit(uint8_t message[], uint16_t message_size);
  void TransmitIsOK();
  void TransmitIsError();

  void Receive();
  void ReceivePacketIsOK();
  void ReceivePacketIsError();

  void LedFlash(uint8_t flashes, uint16_t delay_ms);
  void PrintElapsedTime();


private:
  SX128XLT lora;

  uint8_t tx_packet_length, rx_buffer[255], rx_packet_length, packet_snr;
  uint16_t packet_rssi;
  uint32_t tx_packet_count, rx_packet_count, start_ms, end_ms, error_count;
};

#endif