#define GT_STREAM_MODE    // для работы readString и прочих

#include <GyverTransfer.h>
GyverTransfer<2, GT_RX, 5000, 20> rx;

void setup() {
  Serial.begin(9600);
  attachInterrupt(0, isr, CHANGE);
  rx.setTimeout(100);
}

void isr() {
  // спец. тикер вызывается в прерывании
  rx.tickISR();
}

void loop() {
  if (rx.available()) {
    Serial.print(rx.readString());
  }
}
