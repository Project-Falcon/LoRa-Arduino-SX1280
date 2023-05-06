#define Program_Version "V1.0"

#include <SPI.h>
#include "mySX1280.h"
#include "Settings.h"

uint8_t buff[] = "Hello World 1234567890"; // TODO: What is this used for?

mySX1280::mySX1280() {}

void mySX1280::Setup()
{
  pinMode(TXLED, OUTPUT);
  pinMode(RXLED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.println("SETUP");
  LedFlash(2, 250, 3);
  Buzzer(2, 800, 200);

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
    LedFlash(2, 250, 3);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      LedFlash(10, 100, 3); // long fast speed both LED flash indicates device error
      Buzzer(2, 800, 250);
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

void mySX1280::SetUID(String newUid)
{
  uid = newUid;
}

String mySX1280::GetUID()
{
  return uid;
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
  Serial.print(F("TRANSMIT"));
  Serial.print(F("dBm:"));
  Serial.print(tx_power);
  Serial.print(F(";"));
  Serial.flush();
  Serial.print(F("Packet:"));
  tx_packet_length = message_size;
  message[tx_packet_length - 1] = '*'; // replace null character at buffer end so its visible on reciver
  lora.printASCIIPacket(message, tx_packet_length);
  Serial.print(F(";"));
  digitalWrite(TXLED, HIGH);
  start_ms = millis();
  if (lora.transmit(message, tx_packet_length, 200, tx_power, 0)) // will return packet length sent if OK, otherwise 0 if transmit, timeout 10 seconds
  {
    end_ms = millis(); // packet sent, note end time
    tx_packet_count++;
    TransmitIsOK();
  }
  else
  {
    TransmitIsError(); // transmit packet returned 0, there was an error
  }

  digitalWrite(TXLED, LOW);
  Serial.println();
}

void mySX1280::TransmitIsError()
{
  uint16_t irq_status;
  irq_status = lora.readIrqStatus();
  Serial.print(F("BytesSent:"));
  Serial.print(tx_packet_length);
  Serial.print(F(";"));
  Serial.print(F("IRQReg:"));
  Serial.print(irq_status, HEX);
  Serial.print(F(";"));
  PrintIrqStatus(irq_status);
  Serial.print(F(";"));
  Serial.print(F("Status:ERR"));
}

void mySX1280::TransmitIsOK()
{
  uint16_t loral_crc;

  Serial.print(F("BytesSent:"));
  Serial.print(tx_packet_length);
  Serial.print(F(";"));
  loral_crc = lora.CRCCCITT(buff, tx_packet_length, 0xFFFF);
  Serial.print(F("CRC:"));
  Serial.print(loral_crc, HEX);
  Serial.print(F(";"));
  Serial.print(F("TransmitTime:"));
  Serial.print(end_ms - start_ms);
  Serial.print(F(";"));
  Serial.print(F("Status:OK"));
}

void mySX1280::Receive()
{
  rx_packet_length = lora.receive(rx_buffer, RXBUFFER_SIZE, 300, 1); // wait for a packet to arrive with 60seconds (60000mS) timeout

  digitalWrite(RXLED, HIGH);

  packet_rssi = lora.readPacketRSSI();
  packet_snr = lora.readPacketSNR();

  if (rx_packet_length == 0) // if the lora.receive() function detects an error, rx_packet_length == 0
  {
    ReceivePacketIsError();
  }
  else
  {
    String str = (char *)rx_buffer;

    // need to change to 8
    if (str.substring(0, 4) != GetUID())
    {
      ReceivePacketIsOK();
    }
  }

  digitalWrite(RXLED, LOW);
}

void mySX1280::ReceivePacketIsOK()
{
  uint16_t irq_status, local_crc;

  irq_status = lora.readIrqStatus();

  rx_packet_count++;
  Serial.print(F("RECEIVE"));
  Serial.print(F("Packet:"));
  lora.printASCIIPacket(rx_buffer, rx_packet_length);
  Serial.print(F(";"));
  local_crc = lora.CRCCCITT(rx_buffer, rx_packet_length, 0xFFFF); // calculate the CRC, this is the external CRC calculation of the rx_buffer
  Serial.print(F("CRC:"));                                        // contents, not the LoRa device internal CRC
  Serial.print(local_crc, HEX);
  Serial.print(F(";"));
  Serial.print(F("RSSI:"));
  Serial.print(packet_rssi);
  Serial.print(F(";"));
  Serial.print(F("SNR:"));
  Serial.print(packet_snr);
  Serial.print(F(";"));
  Serial.print(F("BytesRec:"));
  Serial.print(rx_packet_length);
  Serial.print(F(";"));
  Serial.print(F("IRQreg:"));
  Serial.print(irq_status, HEX);
  Serial.print(F(";"));
  Serial.print(F("Status:OK"));
  Serial.println();
}

void mySX1280::ReceivePacketIsError()
{
  uint16_t irq_status;
  irq_status = lora.readIrqStatus();

  if (irq_status & IRQ_RX_TIMEOUT) // check for an RX timeout
  {
    // Serial.print(F(" RXTimeout"));
  }
  else if (irq_status == 0x0001)
  { // check if iqr status is set to only IRQ_TX_DONE
  }
  else
  {
    Serial.print(F("RECEIVE"));
    Serial.print(F("RSSI:"));
    Serial.print(packet_rssi);
    Serial.print(F(";"));
    Serial.print(F("SNR:"));
    Serial.print(packet_snr);
    Serial.print(F(";"));
    Serial.print(F("BytesRec:"));
    Serial.print(lora.readRXPacketL()); // get the real packet length
    Serial.print(F(";"));
    Serial.print(F("IRQReg:"));
    Serial.print(irq_status, HEX);
    Serial.print(F(";"));
    PrintIrqStatus(irq_status); // print the names of the IRQ registers set
    Serial.print(F(";"));
    Serial.print(F("Status:ERR"));
    Serial.println();
  }

  // delay(250);
}

void mySX1280::PrintElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}

void mySX1280::PrintIrqStatus(uint16_t irq_status)
{
  Serial.print(F("IRQStatus:"));
  // 0x0001
  if (irq_status & IRQ_TX_DONE)
  {
    Serial.print(F("IRQ_TX_DONE,"));
  }

  // 0x0002
  if (irq_status & IRQ_RX_DONE)
  {
    Serial.print(F("IRQ_RX_DONE,"));
  }

  // 0x0004
  if (irq_status & IRQ_SYNCWORD_VALID)
  {
    Serial.print(F("IRQ_SYNCWORD_VALID,"));
  }

  // 0x0008
  if (irq_status & IRQ_SYNCWORD_ERROR)
  {
    Serial.print(F("IRQ_SYNCWORD_ERROR,"));
  }

  // 0x0010
  if (irq_status & IRQ_HEADER_VALID)
  {
    Serial.print(F("IRQ_HEADER_VALID,"));
  }

  // 0x0020
  if (irq_status & IRQ_HEADER_ERROR)
  {
    Serial.print(F("IRQ_HEADER_ERROR,"));
  }

  // 0x0040
  if (irq_status & IRQ_CRC_ERROR)
  {
    Serial.print(F("IRQ_CRC_ERROR,"));
  }

  // 0x0080
  if (irq_status & IRQ_RANGING_SLAVE_RESPONSE_DONE)
  {
    Serial.print(F("IRQ_RANGING_SLAVE_RESPONSE_DONE,"));
  }

  // 0x0100
  if (irq_status & IRQ_RANGING_SLAVE_REQUEST_DISCARDED)
  {
    Serial.print(F("IRQ_RANGING_SLAVE_REQUEST_DISCARDED,"));
  }

  // 0x0200
  if (irq_status & IRQ_RANGING_MASTER_RESULT_VALID)
  {
    Serial.print(F("IRQ_RANGING_MASTER_RESULT_VALID,"));
  }

  // 0x0400
  if (irq_status & IRQ_RANGING_MASTER_RESULT_TIMEOUT)
  {
    Serial.print(F("IRQ_RANGING_MASTER_RESULT_TIMEOUT,"));
  }

  // 0x0800
  if (irq_status & IRQ_RANGING_SLAVE_REQUEST_VALID)
  {
    Serial.print(F("IRQ_RANGING_SLAVE_REQUEST_VALID,"));
  }

  // 0x1000
  if (irq_status & IRQ_CAD_DONE)
  {
    Serial.print(F("IRQ_CAD_DONE,"));
  }

  // 0x2000
  if (irq_status & IRQ_CAD_ACTIVITY_DETECTED)
  {
    Serial.print(F("IRQ_CAD_ACTIVITY_DETECTED,"));
  }

  // 0x4000
  if (irq_status & IRQ_RX_TX_TIMEOUT)
  {
    Serial.print(F("IRQ_RX_TX_TIMEOUT,"));
  }

  // 0x8000
  if (irq_status & IRQ_PREAMBLE_DETECTED)
  {
    Serial.print(F("IRQ_PREAMBLE_DETECTED,"));
  }
}
void mySX1280::LedFlash(int flashes, int delay_ms, int led_number)
{
  for (uint8_t i = 0; i < flashes; i++)
  {
    if (led_number == 1 || led_number == 2)
    {
      if (led_number == 1)
      {
        digitalWrite(TXLED, HIGH);
      }
      else if (led_number == 2)
      {
        digitalWrite(RXLED, HIGH);
      }
      delay(delay_ms);

      if (led_number == 1)
      {
        digitalWrite(TXLED, LOW);
      }
      else if (led_number == 2)
      {
        digitalWrite(RXLED, LOW);
      }
      delay(delay_ms);
    }
    else if (led_number == 3)
    {
      Serial.println("LED BLINKING");
      digitalWrite(TXLED, HIGH);
      digitalWrite(RXLED, HIGH);
      delay(50);

      digitalWrite(TXLED, LOW);
      digitalWrite(RXLED, LOW);
      delay(50);
    }
  }
}

void mySX1280::Buzzer(int buzzes, int frequency, uint16_t delay_ms)
{
  for (int i = 0; i < buzzes; i++)
  {
    tone(BUZZER, frequency);
    delay(delay_ms);
    noTone(BUZZER);
    delay(delay_ms);
  }
}
