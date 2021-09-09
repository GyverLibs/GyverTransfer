// читаем любой тип данных средствами библиотеки
// например - структура
struct Data {
  byte valB;
  uint32_t valU;
  float valF;
};

#include <GyverTransfer.h>
GyverTransfer<2, GT_RX, 5000, 20> rx;

void setup() {
  Serial.begin(9600);
  attachInterrupt(0, isr, CHANGE);
}

void isr() {
  // спец. тикер вызывается в прерывании
  rx.tickISR();
}

void loop() {
  // если приняты какие то данные (встроенный таймаут)
  if (rx.gotData()) {
    Data data;

    // прочитать данные, если они
    // приняты корректно и соответствуют размеру
    if (rx.readDataCRC(data)) {
      Serial.println(data.valB);
      Serial.println(data.valU);
      Serial.println(data.valF);
      Serial.println();
    } else {
      // иначе данные повреждены или не той длины!
      
      // сами разбираем если нужно
      // .................      
    }
    rx.clearBuffer(); // обязательно вручную чистим буфер
  }
}
