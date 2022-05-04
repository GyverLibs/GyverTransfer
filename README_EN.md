This is an automatic translation, may be incorrect in some places. See sources and examples!

# GyverTransfer
Library for data transfer via the GyverTransfer (GT) interface
- GT - Reliable single-wire, self-timed pulse-length based communication interface
- Asynchronous receive in CHANGE interrupt
- Speed ​​up to 45'000 baud (over the wire)
- Customizable inheritance of Print and Stream classes
- Customizable receive buffer
- Send without buffer, blocking
- Operating mode of the transmitter, receiver and transceiver
- Supports 38 kHz IR transmission
- Support 433MHz radio transmission
- The library does not take any interruptions, everything is manual
- A light weight

### Compatibility
Compatible with all Arduino platforms (using Arduino functions)
- When connecting an interrupt on esp8266, do not forget the `IRAM_ATTR` attribute

## Content
- [Install](#install)
- [Initialization](#init)
- [Usage](#usage)
- [Example](#example)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="install"></a>
## Installation
- The library can be found by the name **GyverTransfer** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download library](https://github.com/GyverLibs/GyverTransfer/archive/refs/heads/main.zip) .zip archive for manual installation:
    - Unzip and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unzip and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Raspaforge and put in *Documents/Arduino/libraries/*
    - (Arduino IDE) automatic installation from .zip: *Sketch/Include library/Add .ZIP library…* and specify the downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE% D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Initialization
```cpp
GyverTransfer<pin, role> transfer;
GyverTransfer<pin, role, speed> transfer;
GyverTransfer<pin, role, speed, buffer> transfer;
```
- **Pin**: any digital pin. Preferably with a convenient interrupt connection for asynchronous reception
- **Role**:
    - *GT_TX*: transmitter, for wired operation
    - *GT_RX*: receiver, for wired operation
    - *GT_TRX*: transceiver (transceiver), for wired operation
    - *GT_38KHZ*: 38 kHz modulation to send via IR transmitter. Max. speed: 2000 baud
    - *GT_433MHZ*: Adds a set of sync pulses before being sent on the 433MHz radio. Max. speed: 3000 baud
- **Speed**: Speed: 0-45000 baud. By default 5000
- **Buffer**: receiver buffer size (GT_RX/GT_TRX). By default 0
    - Note: The code will run faster if the buffer size is a power of 2

<a id="usage"></a>
## Usage
```cpp
// SEND
void write(uint8_t data); // send data byte
void writeData(T&data); // send data of any type and size
void writeDataCRC(T&data); // send data of any type and size + CRC
// with GT_PRINT_MODE enabled, sends print() any data type and write() arrays of bytes

// RECEPTION
bool tick(); // receive ticker to call in loop() (not recommended). Returns true if a byte is received
bool tickISR(); // receive ticker to call in CHANGE interrupt. Returns true if a byte is received
int available(); // will return the number of bytes in the receive buffer
bool gotData(); // received data (by timeout)
intread(); //read from the receive buffer (-1 if there is nothing to read)
bool readData(T&data); // read buffer into any data type. true if successfully read
bool readDataCRC(T&data); // read buffer into any data type + CRC. true if successfully read
int peek(); // read from the receive buffer without deleting from the buffer
void clearBuffer(); // clear the receive buffer
uint8_tbuffer[]; // access to FIFO receive buffer

// DEFINE SETTINGS (specify before connecting the library)
#define GT_STREAM_MODE // [GT_RX] inherit Print.h and Stream.h (receiving parseInt, readBytes and others)
#define GT_FLOW_CONTROL // [GT_RX] check the parity of the receive. Reduces max. receive speed (up to ~30'000 baud)
#define GT_PRINT_MODE // [GT_TX] extend Print.h (send print any data)
#define GT_MICRO_TX // [GT_TX] cut receiver code to save space
#define GT_CLI // [GT_TX] disable interrupts while sending (improves communication at high speeds when code is loaded)
#define GT_OFFSET (number) // [GT_TX] transmitter delay correction, µs (default 5)
#define GT_TRAINING (number) // [GT_TX] sync duration for GT_433MHZ role in µs, default 100000
#define GT_HIGH_OUT // [GT_TX] HIGH pulse will be in OUTPUT mode instead of INPUT_PULLUP
```

<a id="example"></a>
## Examples
See **examples** for other examples!
### Send via Print
```cpp
#define GT_PRINT_MODE // to send via print
#define GT_MICRO_TX // cut receiver code (weight optimization)

#include <GyverTransfer.h>
GyverTransfer<2, GT_TX, 5000> tx;

void setup() {
}

void loop() {
  tx.println("Hello world 123 test");
  delay(1000);
}
```

### Receiving via Stream
```cpp
#define GT_STREAM_MODE // for readString and others to work

#include <GyverTransfer.h>
GyverTransfer<2, GT_RX, 5000, 20> rx;

void setup() {
  Serial.begin(9600);
  attachInterrupt(0, isr, CHANGE);
  rx.setTimeout(100);
}

void isr() {
  // spec. tickp is called in interrupt
  rx.tickISR();
}

void loop() {
  if (rx.available()) {
    Serial.print(rx.readString());
  }
}
```

### Send manually + CRC
```cpp
// send any type of data using the library
// for example - structure. The receiver knows about it
struct Data {
  byte valB;
  uint32_tvalU;
  float value;
};

#define GT_MICRO_TX // cut receiver code (weight reduction)

#include <GyverTransfer.h>
GyverTransfer<2, GT_TX, 5000> tx;

void setup() {
}

void loop() {
  // we will transfer the counter, millis and pi
  static byte count;
  count++;
  Data data = (Data) {
    count, millis(), 3.14
  };
  
  // send with CRC to be safe
  tx.writeDataCRC(data);
  delay(1000);
}
```

### Receive manually + CRC
```cpp
// read any data type using the library
// for example - structure
struct Data {
  byte valB;
  uint32_tvalU;
  float value;
};

#include <GyverTransfer.h>
GyverTransfer<2, GT_RX, 5000, 20> rx;

void setup() {
  Serial.begin(9600);
  attachInterrupt(0, isr, CHANGE);
}

void isr() {
  // spec. ticker is called in interrupt
  rx.tickISR();
}

void loop() {
  // if some data is received (built-in timeout)
  if (rx.gotData()) {
    data data;

    // read data if it is
    // accepted correctly and fit the size
    if (rx.readDataCRC(data)) {
      Serial.println(data.valB);
      Serial.println(data.valU);
      Serial.println(data.valF);
      Serial.println();
    } else {
      // otherwise the data is corrupted or the wrong length!
      
      // disassemble if necessary
      // .................
    }
    rx.clearBuffer(); // be sure to manually clear the buffer
  }
}
```

<a id="versions"></a>
## Versions
- v1.0
- v1.1 - LOW on line for 433mhz/38khz modes
- v1.2 - minor fixes
- v1.3 - fixed critical bug

<a id="feedback"></a>
## Bugs and feedback
When you find bugs, create an **Issue**, or better, immediately write to the mail [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your**Pull Request**'s!