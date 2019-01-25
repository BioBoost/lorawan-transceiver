#include "lorawan_transceiver.h"
#include "lora_radio_helper.h"

namespace IoT {

  namespace Communication {

    LoRaWANTransceiver::LoRaWANTransceiver(events::EventQueue * eventQueue)
      : lorawan(radio), eventQueue(eventQueue) {

        isConnected = false;
        initialize();
        connect();
    }

    LoRaWANTransceiver::~LoRaWANTransceiver(void) { }

    int LoRaWANTransceiver::initialize(void) {

      // Initialize LoRaWAN stack
      if (lorawan.initialize(eventQueue) != LORAWAN_STATUS_OK) {
        log("LoRaWAN stack initialization failed!\n");
        return false;
      }
      log("LoRaWAN stack initialized\n");

      // Prepare internal callback
      callbacks.events = mbed::callback(this, &LoRaWANTransceiver::internal_event_handler);
      lorawan.add_app_callbacks(&callbacks);

      // Set number of retries in case of CONFIRMED messages
      if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER) != LORAWAN_STATUS_OK) {
        log("Failed to set confirmed message retries\n");
        return -1;
      }

      return 0;
    }

    int LoRaWANTransceiver::connect(void) {
      lorawan_status_t retcode = lorawan.connect();
      if (!(retcode == LORAWAN_STATUS_OK || retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS)) {
        log("Connection error, code = %d\n", retcode);
        return -1;
      }
      log("Connection in progress ...\n");

      return 0;
    }

    int LoRaWANTransceiver::disconnect(void) {
      lorawan_status_t retcode = lorawan.disconnect();
      if (!(retcode == LORAWAN_STATUS_OK)) {
        log("Disconnection error, code = %d\n", retcode);
        return -1;
      }
      log("Disconnecting from the network ...\n");

      return 0;
    }

    LoRaWANTransceiver::SendStatus LoRaWANTransceiver::send(uint8_t * buffer, uint16_t length, uint8_t port=1) {
      if (!isConnected) {
        log("Not connected to LoRaWAN - cannot send\n");
        return NOT_CONNECTED;
      }

      if (length > LORAMAC_PHY_MAXPAYLOAD) {
        log("Cannot send message via LoRaWAN - too large for buffer\n");
        return TOO_LARGE;
      }

      int16_t retcode = lorawan.send(port, buffer, length, MSG_CONFIRMED_FLAG);
      if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
        log("LoRaWAN send would block - skipping send\n");
        return WOULD_BLOCK;
      } else if (retcode < 0) {
        log("Send failed with error code = %d\n", retcode);
      }
      log("%d bytes scheduled for transmission with LoRaWAN\n", retcode);
      
      return SCHEDULED;
    }

    void LoRaWANTransceiver::on_connected(mbed::Callback<void()> cb)           { onConnected = cb; }        
    void LoRaWANTransceiver::on_disconnected(mbed::Callback<void()> cb)        { onDisconnected = cb; }          
    void LoRaWANTransceiver::on_transmitted(mbed::Callback<void()> cb)         { onTransmitted = cb; }          
    void LoRaWANTransceiver::on_transmission_error(mbed::Callback<void()> cb)  { onTransmissionError = cb; }   

    void LoRaWANTransceiver::internal_event_handler(lorawan_event_t event) {
      switch (event) {
        case CONNECTED:
          log("LoRaWAN connection successful\n");
          isConnected = true;
          if (onConnected) {
            onConnected();
          }
          break;
        case DISCONNECTED:
          isConnected = false;
          log("LoRaWAN disconnected from network\n");
          if (onDisconnected) {
            onDisconnected();
          }
          break;
        case TX_DONE:
          log("LoRaWAN message sent succesfully\n");
          if (onTransmitted) {
            onTransmitted();
          }
          break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
          log("LoRaWAN transmission failed with code = %d\n", event);
          if (onTransmissionError) {
            onTransmissionError();
          }
          break;
        case RX_DONE:
          log("LoRaWAN message received\n");
          break;
        case RX_TIMEOUT:
        case RX_ERROR:
          log("LoRaWAN reception failed with code = %d\n", event);
          break;
        case JOIN_FAILURE:
          log("LoRaWAN OTAA failed - check keys\n");
          break;
        case UPLINK_REQUIRED:
          log("LoRaWAN uplink required\n");
          break;
        default:
          log("Unknown event happened\n");
      }
    }

  };

};