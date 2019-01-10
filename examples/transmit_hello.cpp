// This example uses the LoRaWAN Transceiver to send a simple "Hello from Mbed" message.
// The event queue of the main application is shared with the LoRaWAN stack.

#include "mbed.h"
#include "lorawan_transceiver.h"

Serial pc(USBTX, USBRX);
DigitalOut alive(PC_8);
DigitalOut lora_connected(PC_10);

using namespace IoT::Communication;

// Maximum number of events for the event queue.
#define MAX_NUMBER_OF_EVENTS            10
EventQueue ev_queue(MAX_NUMBER_OF_EVENTS * EVENTS_EVENT_SIZE);

LoRaWANTransceiver * lora;

// Alive LED, runs in user mode through event queue
void show_alive(void) {
  alive = !alive;
}

void on_lora_connected(void) {
  lora_connected = 1;
  uint8_t buffer[] = "Hello from Mbed";
  lora->send(buffer, sizeof(buffer)-1, 5);
}

// main() runs in its own thread in the OS
int main(void) {
  pc.baud(115200);
  printf("LoRaWAN Transceiver Demo ...\n");

  lora = new LoRaWANTransceiver(&ev_queue);
  lora->on_connected(callback(on_lora_connected));

  ev_queue.call_every(1000, callback(show_alive));

  // Make event queue dispatch events forever
  ev_queue.dispatch_forever();
}