#pragma once

#include "lorawan/LoRaWANInterface.h"
#include "events/EventQueue.h"
#include "platform/Callback.h"

#ifndef ENABLE_LORA_LOGGING
  #define log(f_, ...) while(false)
#else
  #define log(f_, ...) printf((f_), ##__VA_ARGS__)
#endif

namespace IoT {

  namespace Communication {

    class LoRaWANTransceiver {

      public:
        enum SendStatus {
          SCHEDULED,
          WOULD_BLOCK,
          TOO_LARGE,
          NOT_CONNECTED
        };

      public:
        LoRaWANTransceiver(events::EventQueue * eventQueue);
        virtual ~LoRaWANTransceiver(void);

      public:
        int connect(void);
        int disconnect(void);

      public:
        SendStatus send(uint8_t * buffer, uint16_t length, uint8_t port);

      public:
        void on_connected(mbed::Callback<void()> cb);
        void on_disconnected(mbed::Callback<void()> cb);
        void on_transmitted(mbed::Callback<void()> cb);
        void on_transmission_error(mbed::Callback<void()> cb);

      private:
        int initialize(void);
        void internal_event_handler(lorawan_event_t event);

      private:
        // LoRaWANInterface will construct PHY based on "lora.phy" setting in mbed_app.json. 
        LoRaWANInterface lorawan;

        // Event queue used by stack for ISR deferment
        events::EventQueue * eventQueue;

        // Application specific callbacks
        lorawan_app_callbacks_t callbacks;

        // Allows for easy event handler registration
        mbed::Callback<void()> onConnected;
        mbed::Callback<void()> onDisconnected;
        mbed::Callback<void()> onTransmitted;
        mbed::Callback<void()> onTransmissionError;

        // Track if connected
        bool isConnected;
    };

  };

};