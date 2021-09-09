// отправляем любой тип данных средствами библиотеки
// например - структура. Приёмник знает об этом
struct Data {
  byte valB;
  uint32_t valU;
  float valF;
};

#define GT_MICRO_TX     // вырезать код приёмника (уменьшение веса)

#include <GyverTransfer.h>
GyverTransfer<2, GT_TX, 5000> tx;

void setup() {
}

void loop() {
  // передавать будем счётчик, миллис и Пи
  static byte count;
  count++;
  Data data = (Data) {
    count, millis(), 3.14
  };
  
  // отправляем с CRC для надёжности
  tx.writeDataCRC(data);
  delay(1000);
}
