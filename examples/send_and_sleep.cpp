// This example uses the LoRaWAN Transceiver to send a simple "Hello from Mbed" message
// and the go to sleep for some time, waking up using a low power timer.
// The event queue of the main application is shared with the LoRaWAN stack.

#include "mbed.h"
#include "lorawan_transceiver.h"

Serial pc(USBTX, USBRX);
DigitalOut alive(LED3);
DigitalOut lora_connected(LED2);
DigitalOut sleeping(LED1);

LowPowerTimeout wakeupTimer;

volatile bool do_sleep = false;

using namespace IoT::Communication;

// Maximum number of events for the event queue.
#define MAX_NUMBER_OF_EVENTS            10
EventQueue * ev_queue = NULL;

LoRaWANTransceiver * lora = NULL;

// Alive LED, runs in user mode through event queue
void show_alive(void) {
  alive = !alive;
}

void on_lora_connected(void) {
  lora_connected = 0;
  uint8_t buffer[] = "Hello from Mbed";
  lora->send(buffer, sizeof(buffer)-1, 5);
}

void on_lora_transmitted(void) {
  printf("Disconnecting ...\n");
  lora->disconnect();
}

void on_lora_disconnected(void) {
  printf("Disconnected\n");
  lora_connected = 1;
  do_sleep = true;
}

void setup_lora(void) {
  printf("Setting up LoRaWAN\n");
  ev_queue = new EventQueue(MAX_NUMBER_OF_EVENTS * EVENTS_EVENT_SIZE);
  ev_queue->call_every(1000, callback(show_alive));
  lora = new LoRaWANTransceiver(ev_queue);
  lora->on_connected(callback(on_lora_connected));
  lora->on_disconnected(callback(on_lora_disconnected));
  lora->on_transmitted(callback(on_lora_transmitted));
}

void breakdown_lora(void) {
  ev_queue->break_dispatch();
  delete lora;
  lora = NULL;
  delete ev_queue;
  ev_queue = NULL;
}

void wake_up(void) {
  do_sleep = false;
}

// main() runs in its own thread in the OS
int main(void) {
  pc.baud(115200);
  lora_connected = 1;
  sleeping = 1;
  printf("LoRaWAN Transceiver Demo ...\n");

  while(true) {
    sleeping = 1;
    setup_lora();
    while(!do_sleep) {
      ev_queue->dispatch(250);
    }
    printf("Breaking LoRa\n");
    breakdown_lora();
    printf("Going to sleep\n");
    sleeping = 0;
    wakeupTimer.attach(wake_up, 30.0f);
    while(do_sleep) {
      sleep();
    }
  }
}