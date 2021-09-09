#define GT_PRINT_MODE   // для отправки через print
//#define GT_CLI        // для лучшей связи на высокой скорости
#define GT_MICRO_TX     // вырезать код приёмника (оптимизация веса)

#include <GyverTransfer.h>
GyverTransfer<2, GT_TX, 5000> tx;

void setup() {
}

void loop() {
  tx.println("Hello world 123 test");
  delay(1000);
}
