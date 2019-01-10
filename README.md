# Mbed LoRaWAN Transceiver Library

An simple wrapper around the LoRaWAN classes of Mbed. It provides a simpler callback mechanism and allows for easy sending and receiving of messages.

## Dependencies

Depends on the Semtech LoRa/FSK radio driver library which can be found at [https://github.com/ARMmbed/mbed-semtech-lora-rf-drivers](https://github.com/ARMmbed/mbed-semtech-lora-rf-drivers). This library provides drivers for both the Semtech SX1272 and SX1276 LoRa radio chips.

The SX1276 is found on for example the RFM95W module commonly used. This chip does not implement the LoRaWAN protocol stack (in contrast to for example the Microchip RN2483).

| Microchip RN2483 | RFM95W |
|     :---:      |     :---:      |
| ![](img/rn2483.png) | ![](img/RFM95W.png) |
| with On-board LoRaWAN protocol stack | LoRa Radio transceiver, requires LoRaWAN software stack |