This is an automatic translation, may be incorrect in some places. See sources and examples!

# Gyvertransfer
Library for transmitting data on the GyverTransfer interface (GT)
- GT is a reliable one -wire self -synchronizing communication interface based on the length of the pulse
- asynchronous reception in interruption by Change
- speed up to 45'000 body (by wire)
- customizer inheritance of Print and Stream classes
- Completed buffer for reception
- Sending without a buffer, blocking
- operating mode of the transmitter, receiver and transceiver
- Support for transmission by IR channel 38 kHz
- Support for gear on radio 433 MHz
- The library does not take any interruptions, everything manually
- A light weight
! [GT] (/doc/interface.png)

## compatibility
Compatible with all arduino platforms (used arduino functions)
- When connecting an interruption on the ESP8266, do not forget the Attricbut `IRAM_ATTR`

## Content
- [installation] (# Install)
- [initialization] (#init)
- [use] (#usage)
- [Example] (# Example)
- [versions] (#varsions)
- [bugs and feedback] (#fedback)

<a id="install"> </a>
## Installation
- The library can be found by the name ** gyvertransfer ** and installed through the library manager in:
    - Arduino ide
    - Arduino ide v2
    - Platformio
- [download the library] (https://github.com/gyverlibs/gyvertransfer/archive/refs/heads/main.zip). Zip archive for manual installation:
    - unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
    - unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
    - unpack and put in *documents/arduino/libraries/ *
    - (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!


<a id="init"> </a>
## initialization
`` `CPP
Gyvertransfer <Pin, Role> Transfer;
Gyvertransfer <Pin, Role, Speed> Transfer;
Gyvertransfer <PIN, Role, Speed, Boofer> Transfer;
`` `
- ** PIN **: Any digital pin.Preferably with a convenient interruption connection for asynchronous intake
- ** Role **:
    - *gt_tx *: transmitter, for work on the wire
    - *gt_rx *: receiver, for work on the wire
    - *gt_trx *: receiver (transceiver), for work on the wire
    - *gt_38khz *: modulation of 38 kHz for sending through IR transmitter.Max.Speed: 2000 Bod
    - *GT_433MHz *: adds a set of synchroimpuls before sending 433 MHz before the radio.Max.Speed: 3000 BOD
- ** speed **: speed: 0-45000 baud.By the silence.5000
- ** buffer **: the size of the receiver buffer (gt_rx/gt_trx).By the silence.0
    - Note: the code will work faster with the size of the buffer, a multiple of 2

<a id="usage"> </a>
## POlse
`` `CPP
// Sending
VOID Write (Uint8_T DATA);// Send byte data
VOID Writededata (T & Data);// Send data any type and size
VOID Writededacrc (T & Data);// Send data of any type and size + CRC
// when GT_PRINT_MODE is on, it sends print () any type of data and WRITE () bytes of the byte

// Reception
Bool Tick ();// Take of reception for calling in LOOP () (not recommended).Will return True if byte is accepted
Bool Tickisr ();// Take of reception for calling in interruption in Change.Will return True if byte is accepted
int Available ();// will return the number of bytes in the reception buffer
Bool Gotdata ();// received data (by timeout)
int Read ();// read from a buffer of the reception (-1 if there is nothing to read)
Bool Readdata (T & Data);// Read the buffer in any type of data.True if you read it successfully
Bool Readdatacrc (T & Data);// Read the buffer in any type of data + CRC.True if you read it successfully
int peek ();// Read from the reception buffer without removal from the buffer
Void Clearbuffer ();// Clean the reception buffer
uint8_t buffer [];// access to fifo reception buffer

// defines of settings (indicate before connecting the library)
#define gt_stream_mode // [gt_rx] inherit Print.h and Stream.h (Reception Parseint, Readbytes and others)
#define gt_flow_control // [gt_rx] Check the time of reception.Reduces max.Reception speed (up to ~ 30'000 BOD)
#define gt_print_mode // [gt_tx] inherit print.h (sending Print of any data)
#define gt_micro_tx // [gt_tx] cuts out a receiver code to save space
#define gt_cli // [gt_tx] Prohibition of interruptions for the duration of sending (improving communication at high speeds at a loaded code)
#define gt_offset (number) // [gt_tx] Correction of delay for the transmitter, ISS (by default. 5)
#define gt_training (number) // [gt_tx] The duration of synchroimpuls for the role of GT_433MHz in the ISS, by the silence.100000
#define gt_high_out // [gt_tx] High impulse will be in Output mode instead of input_pullup
`` `

<a id="EXAMPLE"> </a>
## Examples
The rest of the examples look at ** Examples **!
### Sending through Print
`` `CPP
#define gt_print_mode // for sending through Print
#define gt_micro_tx // Cut the receiver code (weight optimization)

#include <gyvertransfer.h>
Gyvertransfer <2, gt_tx, 5000> tx;

VOID setup () {
}

VOID loop () {
  tx.println ("Hello World 123 Test");
  DELAY (1000);
}
`` `

### Reception through Stream
`` `CPP
#define gt_stream_mode // for the work of Readstring and others

#include <gyvertransfer.h>
Gyvertransfer <2, gt_rx, 5000, 20> rx;

VOID setup () {
  Serial.Begin (9600);
  Attachinterrupt (0, Isr, Change);
  rx.settimeout (100);
}

VOID isr () {
  // Special.The ticer is called in interruption
  rx.tickisr ();
}

VOID loop () {
  if (rx.available ()) {
    Serial.print (rx.readstring ());
  }
}
`` `

### Sending manually + CRC
`` `CPP
// Send any type of data from the library
// for example - structure.The receiver knows about this
Struct Data {
  byte valb;
  uint32_t valu;
  Float VALF;
};

#define gt_micro_tx // Cut the receiver code (weight loss)

#include <gyvertransfer.h>
Gyvertransfer <2, gt_tx, 5000> tx;

VOID setup () {
}

VOID loop () {
  // we will transmit the counter, millis and pi
  Static Byte Count;
  Count ++;
  Data Data = (Data) {
    Count, Millis (), 3.14
  };
  
  // Send from CRC for reliability
  TX.Writededacrc (Data);
  DELAY (1000);
}
`` `

### Making manually + CRC
`` `CPP
// Read any type of data by library
// for example - structure
Struct Data {
  byte valb;
  uint32_t valu;
  Float VALF;
};

#include <gyvertransfer.h>
Gyvertransfer <2, gt_rx, 5000, 20> rx;

VOID setup () {
  Serial.Begin (9600);
  Attachinterrupt (0, Isr, Change);
}

VOID isr () {
  // Special.The ticer is called in interruption
  rx.tickisr ();
}

VOID loop () {
  // if any data is accepted (built -in timaut)
  if (rx.gotdata ()) {
    Data Data;

    // Read the data if they
    // accepted correctly and correspond to the size
    if (rx.reAddatacrc (Data)) {
      Serial.println (Data.valb);
      Serial.println (Data.valu);
      Serial.println (Data.valf);
      Serial.println ();
    } else {
      // Otherwise, the data is damaged or the wrong length!
      
      // ourselves analyze if necessary
      // .................
    }
    rx.clearbuffer ();// be sure to manually clean the buffer
  }
}
`` `

<a id="versions"> </a>
## versions
- V1.0
- V1.1 - Low on the line for 433MHZ/38KHZ modes
- V1.2 - Small fixes
- V1.3 - Critical error is fixed
- v1.3.1 - Fix Compiler Warnings

<a id="feedback"> </a>
## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!


When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code