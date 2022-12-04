/*
    Библиотека для передачи данных по интерфейсу GyverTransfer (GT)
    Документация: 
    GitHub: https://github.com/GyverLibs/GyverTransfer
    Возможности:
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
    
    AlexGyver, alex@alexgyver.ru
    https://alexgyver.ru/
    MIT License

    Версии:
    v1.0
    v1.1 - LOW на линии для 433mhz/38khz режимов
    v1.2 - мелкие фиксы
    v1.3 - исправлена критическая ошибка
    v1.3.1 - fix compiler warnings
*/

#ifndef _GyverTransfer_h
#define _GyverTransfer_h
#include <Arduino.h>

// роли
#define GT_TX 0
#define GT_RX 1
#define GT_TRX 2
#define GT_38KHZ 3
#define GT_433MHZ 4

// тайминги интерфейса
#define GT_HIGH(x) (1000000ul / (x))
#define GT_LOW(x) (GT_HIGH(x) * 1 / 2)
#define GT_START(x) (GT_HIGH(x) * 3 / 2)
#define GT_WINDOW(x) (GT_HIGH(x) / 4)
#define GT_EDGE_L(x) (GT_LOW(x) - GT_WINDOW(x))
#define GT_EDGE_LH(x) (GT_HIGH(x) - GT_WINDOW(x))
#define GT_EDGE_HS(x) (GT_HIGH(x) + GT_WINDOW(x))
#define GT_EDGE_S(x) (GT_START(x) + GT_WINDOW(x))

// умолчания
#ifndef GT_OFFSET
#define GT_OFFSET 6
#endif

#ifndef GT_TRAINING
#define GT_TRAINING 150000
#endif

// жоский delay для avr
#ifdef AVR
#define GT_DELAY(x) _delay_us(x)
#else
#define GT_DELAY(x) delayMicroseconds(x)
#endif

// virtual закрыты для экономии 2х байт срамы

template <uint8_t GT_PIN, uint8_t GT_ROLE, uint16_t GT_SPEED = 5000, uint16_t GT_RXBUF = 0>
class GyverTransfer
#ifdef GT_STREAM_MODE
#undef GT_PRINT_MODE
: public Stream
#endif
#ifdef GT_PRINT_MODE
: public Print
#endif
{
public:
    // =============================== CONSTRUCTOR ===============================
    GyverTransfer() {
        if (GT_ROLE == GT_38KHZ || GT_ROLE == GT_433MHZ) pinMode(GT_PIN, OUTPUT);
        else if (GT_ROLE == GT_TX || GT_ROLE == GT_TRX) {
        #ifndef GT_HIGH_OUT
            pinMode(GT_PIN, INPUT_PULLUP);
        #else
            pinMode(GT_PIN, OUTPUT);
        #endif
        } else pinMode(GT_PIN, INPUT);
    }
    
    // ================================== WRITE ==================================
    // отправка одного байта
#if defined(GT_STREAM_MODE) || defined(GT_PRINT_MODE)
    virtual size_t
#else
    void
#endif
    write(uint8_t data) {
        if (GT_ROLE == GT_TRX) {    // если трансивер
            while (parse == 1);     // ждём окончания приёма
            transmitting = 1;       // флаг на передачу
        }
        // запрещаем прерывания
        #ifdef GT_CLI
        #ifdef AVR
        uint8_t sregSave = SREG;
        #endif
        noInterrupts();
        #endif
        
        bool flag = 0;
        if (GT_ROLE == GT_38KHZ) {
            // ===== передача байта 38KHZ =====
            pulse38kHz(GT_START(GT_SPEED) - GT_OFFSET);            
            for (uint8_t b = 0; b < 8; b++) {                
                if (data & 1) {                
                    if (flag) pulse38kHz(GT_HIGH(GT_SPEED));
                    else GT_DELAY(GT_HIGH(GT_SPEED));
                } else {                
                    if (flag) pulse38kHz(GT_LOW(GT_SPEED));
                    else GT_DELAY(GT_LOW(GT_SPEED));
                }
                flag = !flag;
                data >>= 1;
            }
            // ===== передача байта 38KHZ завершена =====
        } else {
            // ===== передача байта обычная =====
            setPin(0);
            GT_DELAY(GT_START(GT_SPEED) - GT_OFFSET);                   // START пульс
            setPin(1);            
            for (uint8_t b = 0; b < 8; b++) {            
                if (data & 1) GT_DELAY(GT_HIGH(GT_SPEED) - GT_OFFSET);  // HIGH пульс
                else GT_DELAY(GT_LOW(GT_SPEED) - GT_OFFSET);            // LOW пульс
                setPin(flag);
                flag = !flag;
                data >>= 1;
            }            
            // ===== передача байта завершена =====
        }
        GT_DELAY(GT_LOW(GT_SPEED));                                     // LOW пульс (стоп бит)
        if (GT_ROLE == GT_TRX) transmitting = 0;                        // передача окончена
        
        // разрешаем прерывания
        #ifdef GT_CLI
        #ifdef AVR
        SREG = sregSave;
        #else
        interrupts();
        #endif 
        #endif

#if defined(GT_STREAM_MODE) || defined(GT_PRINT_MODE)
        return 1;
#endif
    }

    // =============================== WRITE BYTES ===============================
    // отправка байтов
#if defined(GT_STREAM_MODE) || defined(GT_PRINT_MODE)
    virtual size_t
#else
    void
#endif
    write(const uint8_t *buffer, size_t size) {
        if (GT_ROLE == GT_433MHZ) {       
            bool flag = 1;
            int pulses = (GT_TRAINING / (2 * GT_START(GT_SPEED))) | 1;
            for (uint16_t i = 0; i < pulses; i++) {
                fastWrite(GT_PIN, flag);
                flag = !flag;
                GT_DELAY(2 * GT_START(GT_SPEED));
            }
        }
        for (uint16_t i = 0; i < size; i++) write(buffer[i]);
#if defined(GT_STREAM_MODE) || defined(GT_PRINT_MODE)
        return 1;
#endif
    }

    // =============================== WRITE DATA ===============================
    // отправка любого типа данных
    template <typename T>
    void writeData(T &data) {
        uint8_t *ptr = (uint8_t*) &data;
        write(ptr, sizeof(T));
        if (GT_ROLE == GT_433MHZ) setPin(0);
    }
    
    // отправка любого типа данных + CRC
    template <typename T>
    void writeDataCRC(T &data) {
        uint8_t *ptr = (uint8_t*) &data;
        uint8_t crc = 0;
        for (uint16_t i = 0; i < sizeof(T); i++) crc8_byte(crc, *(ptr + i));
        write(ptr, sizeof(T));
        write(crc);
        write(~crc);
        if (GT_ROLE == GT_433MHZ) setPin(0);
    }    
    
    // ================================== TICK ==================================
#ifndef GT_MICRO_TX
    // тикер приёма для вызова в loop() (не рекомендуется, используй tickISR)
    bool tick() {
        if (fastRead(GT_PIN) != prevBit) {
            prevBit = !prevBit;
            return tickISR();
        }
        return false;
    }
    
    // тикер приёма для вызова в прерывании по CHANGE. Вернёт true если принят байт
    bool tickISR() {
        if (GT_ROLE == GT_TRX && transmitting) return 0;            // отключаем приём, если трансивер передаёт
        uint32_t pulse = micros() - tmr;                            // считаем время импульса
        tmr += pulse;                                               // сброс таймера. Равносильно tmr = micros()
        if (pulse <= GT_EDGE_L(GT_SPEED)) return (parse = 0);       // импульс слишком короткий
        else if (pulse <= GT_EDGE_HS(GT_SPEED) && parse) {          // окно данных
        #ifdef GT_FLOW_CONTROL
            if (fastRead(GT_PIN) != (bits & 1)) return (parse = 0); // не наш фронт, выходим
        #endif
            byteBuf >>= 1;                                          // двигаем байт
            if (pulse > GT_EDGE_LH(GT_SPEED)) byteBuf |= (1 << 7);  // пишем бит
            if (++bits == 8) {                                      // приняли байт
                if (GT_RXBUF == 0) {                                // буфера нет
                    buffer[0] = byteBuf;                            // записываем в буфер
                    got = 1;                                        // флаг на приём
                } else {                                            // буфер есть
                    uint8_t i = (head + 1) % GT_RXBUF;              // следующая позиция в буфере
                    if (i != tail) {                                // ещё есть место
                        buffer[head] = byteBuf;                     // пишем
                        head = i;                                   // запомнили
                    }
                }
                parse = 0;                                          // парсинг окончен
                return 1;                                           // байт принят
            }
            return 0;
        } else if (pulse <= GT_EDGE_S(GT_SPEED)) {                  // окно START        
            startFlag = true;                                       // старт флаг
            parse = bits = 0;                                       // прерываем парсинг, если он был
            if (fastRead(GT_PIN)) parse = 1;                        // старт бит, начинаем парсинг
            return 0;
        } else return (parse = 0);                                  // слишком длинный импульс, выходим
        return 0;
    }

    // ================================== STREAM ==================================
    // вернёт количество байт в буфере приёма
#ifdef GT_STREAM_MODE
    virtual
#endif
    int available() {
        if (GT_RXBUF == 0) return got;
        else return (GT_RXBUF + head - tail) % GT_RXBUF;
    }
    
    // прочитать из буфера приёма
#ifdef GT_STREAM_MODE
    virtual
#endif
    int read() {        
        if (GT_RXBUF == 0) {
            got = 0;
            return buffer[0];
        } else {
            if (head == tail) return -1;
            uint8_t val = buffer[tail];
            tail = (tail + 1) % GT_RXBUF;
            return val;
        }
    }
    
    // прочитать из буфера приёма без удаления из буфера
#ifdef GT_STREAM_MODE
    virtual
#endif
    int peek() {
        return buffer[tail];
    }
    
    // очистить буфер приёма
    void clearBuffer() {
        head = tail = 0;
    }
    
    // ================================== READ ==================================
    // прочитать буфер в любой тип данных
    template <typename T>
    bool readData(T &data) {
        if (sizeof(T) != available()) return false;     // в буфере нет столько байт
        uint8_t *ptr = (uint8_t*) &data;
        for (uint16_t i = 0; i < sizeof(T); i++) *ptr++ = buffer[(tail + i) % GT_RXBUF];
        return true;
    }    
    
    // прочитать буфер в любой тип данных + CRC
    template <typename T>
    bool readDataCRC(T &data) {
        if (sizeof(T) + 2 != available()) return false;             // в буфере нет столько байт
        uint8_t crc = buffer[(tail + sizeof(T)) % GT_RXBUF];        // позиция crc
        uint8_t icrc = buffer[(tail + sizeof(T) + 1) % GT_RXBUF];   // позиция ~crc
        if (crc & icrc) return false;                               // ошибка передачи crc

        uint8_t *ptr = (uint8_t*) &data;
        icrc = 0;
        for (uint16_t i = 0; i < sizeof(T); i++) {
            *ptr = buffer[(tail + i) % GT_RXBUF];
            crc8_byte(icrc, *ptr++);
        }
        if (crc != icrc) return false;                              // не совпал последний байт crc
        clearBuffer();
        return true;                                                // всё ок
    }
    
    // получены данные (по таймауту)
    bool gotData() {
        if (startFlag) {
            tmr2 = micros();
            return startFlag = false;
        }
        // таймаут как максимальное время передачи байта +100 мкс на случай пауз в отправках
        return (available() && micros() - tmr2 > GT_START(GT_SPEED) + 8 * GT_HIGH(GT_SPEED) + 100);
    }

#ifdef GT_STREAM_MODE
    using Print::write;
#endif

#endif  // GT_MICRO_TX

    // =============================== PRIVATE ===============================
private:
    void setPin(bool state) {
        if (GT_ROLE == GT_38KHZ || GT_ROLE == GT_433MHZ) {
            fastWrite(GT_PIN, state);
        } else {
        #ifndef GT_HIGH_OUT
            if (state) pinMode(GT_PIN, INPUT_PULLUP);
            else {
                pinMode(GT_PIN, OUTPUT);
                fastWrite(GT_PIN, 0);
            }
        #else
            fastWrite(GT_PIN, state);
        #endif
        }        
    }
    
    void pulse38kHz(int dur) {
        dur = (dur / 13) & ~1;   // кол-во пакетов по 13мкс + округляем до нечётного
        bool flag = 1;
        for (int i = 0; i < dur; i++) {
            fastWrite(GT_PIN, flag);
            flag = !flag;
            GT_DELAY(12.5);
        }
    }
    
    bool fastRead(const uint8_t pin) {
    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        if (pin < 8) return bitRead(PIND, pin);
        else if (pin < 14) return bitRead(PINB, pin - 8);
        else if (pin < 20) return bitRead(PINC, pin - 14);
    #elif defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny13__)
        return bitRead(PINB, pin);
    #else
        return digitalRead(pin);
    #endif
        return 0;
    }

    void fastWrite(const uint8_t pin, bool val) {
    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        if (pin < 8) bitWrite(PORTD, pin, val);
        else if (pin < 14) bitWrite(PORTB, (pin - 8), val);
        else if (pin < 20) bitWrite(PORTC, (pin - 14), val);
    #elif defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny13__)
        bitWrite(PORTB, pin, val);
    #else
        digitalWrite(pin, val);
    #endif
    }
    
    void crc8_byte(uint8_t &crc, uint8_t data) {
    #if defined (__AVR__)
        // резкий алгоритм для AVR
        uint8_t counter;
        uint8_t buffer;
        asm volatile (
        "EOR %[crc_out], %[data_in] \n\t"
        "LDI %[counter], 8          \n\t"
        "LDI %[buffer], 0x8C        \n\t"
        "_loop_start_%=:            \n\t"
        "LSR %[crc_out]             \n\t"
        "BRCC _loop_end_%=          \n\t"
        "EOR %[crc_out], %[buffer]  \n\t"
        "_loop_end_%=:              \n\t"
        "DEC %[counter]             \n\t"
        "BRNE _loop_start_%="
        : [crc_out]"=r" (crc), [counter]"=d" (counter), [buffer]"=d" (buffer)
        : [crc_in]"0" (crc), [data_in]"r" (data)
        );
    #else
        // обычный для всех остальных
        uint8_t i = 8;
        while (i--) {
            crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
            data >>= 1;
        }
    #endif
    }

    // =============================== VARS ===============================
    bool transmitting = 0;
    uint8_t parse = 0;
#ifndef GT_MICRO_TX
    uint8_t buffer[GT_RXBUF];
    volatile bool startFlag = false;
    volatile uint8_t byteBuf;
    volatile uint8_t head = 0, tail = 0;
    volatile uint8_t prevBit = 0, bits = 0, got = 0;
    volatile uint32_t tmr;
    uint32_t tmr2;
#endif
};

#endif