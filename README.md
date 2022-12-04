[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverTransfer.svg?color=brightgreen)](https://github.com/GyverLibs/GyverTransfer/releases/latest/download/GyverTransfer.zip)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverTransfer?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverTransfer
Библиотека для передачи данных по интерфейсу GyverTransfer (GT)
- GT - надёжный однопроводной самосинхронизирующийся интерфейс связи, основанный на длине импульса
- Асинхронный приём в прерывании по CHANGE
- Скорость до 45'000 бод (по проводу)
- Настраиваемое наследование классов Print и Stream
- Настраиваемый буфер на приём
- Отправка без буфера, блокирующая
- Режим работы передатчика, приёмника и трансивера
- Поддержка передачи по ИК каналу 38 кГц
- Поддержка передачи по радио 433 МГц
- Библиотека не забирает никаких прерываний, всё вручную
- Лёгкий вес

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)
- При подключении прерывания на esp8266 не забудь аттрибут `IRAM_ATTR`

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **GyverTransfer** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverTransfer/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!


<a id="init"></a>
## Инициализация
```cpp
GyverTransfer<пин, роль> transfer;
GyverTransfer<пин, роль, скорость> transfer;
GyverTransfer<пин, роль, скорость, буфер> transfer;
```
- **Пин**: любой цифровой пин. Желательно с удобным подключением прерывания для асинхронного приёма
- **Роль**:
    - *GT_TX*: передатчик, для работы по проводу
    - *GT_RX*: приёмник, для работы по проводу
    - *GT_TRX*: приёмопередатчик (трансивер), для работы по проводу
    - *GT_38KHZ*: модуляция 38 кГц для отправки через ИК передатчик. Макс. скорость: 2000 бод
    - *GT_433MHZ*: добавляет набор синхроимпульсов перед отправкой по радио 433 МГц. Макс. скорость: 3000 бод
- **Скорость**: скорость: 0-45000 baud. По умолч. 5000
- **Буфер**: размер буфера приёмника (GT_RX/GT_TRX). По умолч. 0
    - Примечание: код будет работать быстрее при размере буфера, кратном степени 2

<a id="usage"></a>
## Использование
```cpp
// ОТПРАВКА
void write(uint8_t data);   // отправить байт данных
void writeData(T &data);    // отправить данные любого типа и размера
void writeDataCRC(T &data); // отправить данные любого типа и размера + CRC
// при включенном GT_PRINT_MODE отправляет print() любой тип данных и write() массивы байт

// ПРИЁМ
bool tick();                // тикер приёма для вызова в loop() (не рекомендуется). Вернёт true если принят байт
bool tickISR();             // тикер приёма для вызова в прерывании по CHANGE. Вернёт true если принят байт
int available();            // вернёт количество байт в буфере приёма
bool gotData();             // получены данные (по таймауту)
int read();                 // прочитать из буфера приёма (-1 если читать нечего)
bool readData(T &data);     // прочитать буфер в любой тип данных. true если успешно прочитали
bool readDataCRC(T &data);  // прочитать буфер в любой тип данных + CRC. true если успешно прочитали
int peek();                 // прочитать из буфера приёма без удаления из буфера
void clearBuffer();         // очистить буфер приёма
uint8_t buffer[];           // доступ к FIFO буферу приёма

// ДЕФАЙНЫ НАСТРОЕК (указывать перед подключением библиотеки)
#define GT_STREAM_MODE      // [GT_RX] наследовать Print.h и Stream.h (приём parseInt, readBytes и прочие)
#define GT_FLOW_CONTROL     // [GT_RX] проверять чётность приёма. Снижает макс. скорость приёма (до ~30'000 бод)
#define GT_PRINT_MODE       // [GT_TX] наследовать Print.h (отправка print любых данных)
#define GT_MICRO_TX         // [GT_TX] вырезает код приёмника для экономии места
#define GT_CLI              // [GT_TX] запрет прерываний на время отправки (улучшение связи на высоких скоростях при загруженном коде)
#define GT_OFFSET (число)   // [GT_TX] коррекция задержки для передатчика, мкс (по умолч. 5)
#define GT_TRAINING (число) // [GT_TX] продолжительность синхроимпульсов для роли GT_433MHZ в мкс, по умолч. 100000
#define GT_HIGH_OUT         // [GT_TX] HIGH импульс будет в режиме OUTPUT вместо INPUT_PULLUP
```

<a id="example"></a>
## Примеры
Остальные примеры смотри в **examples**!
### Отправка через Print
```cpp
#define GT_PRINT_MODE   // для отправки через print
#define GT_MICRO_TX     // вырезать код приёмника (оптимизация веса)

#include <GyverTransfer.h>
GyverTransfer<2, GT_TX, 5000> tx;

void setup() {
}

void loop() {
  tx.println("Hello world 123 test");
  delay(1000);
}
```

### Приём через Stream
```cpp
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
```

### Отправка вручную + CRC
```cpp
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
```

### Приём вручную + CRC
```cpp
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
```

<a id="versions"></a>
## Версии
- v1.0
- v1.1 - LOW на линии для 433mhz/38khz режимов
- v1.2 - мелкие фиксы
- v1.3 - исправлена критическая ошибка
- v1.3.1 - fix compiler warnings

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!


При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
