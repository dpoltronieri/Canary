#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

namespace {
enum serialPortType : const uint8_t {
    HARDWARE,
    SOFTWARE
};
}

class PrintManager {
public:
    PrintManager(HardwareSerial * serialPortHW, size_t messageSize = 128)
        : _message { (uint16_t * const) malloc(sizeof(uint16_t) * messageSize) },
    _serialPortType { HARDWARE },
    _serialPortHW { serialPortHW },
    _serialPortSW { NULL } { };

    PrintManager(SoftwareSerial * serialPortSW, size_t messageSize = 128)
        : _message { (uint16_t * const) malloc(sizeof(uint16_t) * messageSize) },
    _serialPortType { SOFTWARE },
    _serialPortHW { NULL },
    _serialPortSW { serialPortSW } { };

    uint8_t sendData();

    void addValue(const char * fmt, ...); // variadic

protected:
    uint16_t * const _message; // The pointer cant be changed // Descobrir se char nao seria melhor
    uint8_t _serialPortType;
    HardwareSerial * _serialPortHW;
    SoftwareSerial * _serialPortSW;
};


#endif // ifndef PRINTMANAGER_H
