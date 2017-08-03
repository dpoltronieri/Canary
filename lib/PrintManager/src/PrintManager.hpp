#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LogManager.hpp>

namespace {
enum serialPortType : const uint8_t {
    HARDWARE,
    SOFTWARE
};
}

class PrintManager {
public:
    PrintManager(HardwareSerial * serialPortHW)
        : _message { "" }, // (unsigned char * const) malloc(sizeof(unsigned char) * messageSize)
    _serialPortType { HARDWARE },
    _serialPortHW { serialPortHW },
    _serialPortSW { NULL },
    _sdLogManager { NULL } { };

    PrintManager(SoftwareSerial * serialPortSW)
        : _message { "" },
    _serialPortType { SOFTWARE },
    _serialPortHW { NULL },
    _serialPortSW { serialPortSW },
    _sdLogManager { NULL }  { };

    PrintManager(HardwareSerial * serialPortHW, LogManager * sdLogManager)
        : _message { "" }, // (unsigned char * const) malloc(sizeof(unsigned char) * messageSize)
    _serialPortType { HARDWARE },
    _serialPortHW { serialPortHW },
    _serialPortSW { NULL },
    _sdLogManager { sdLogManager } { };

    PrintManager(SoftwareSerial * serialPortSW, LogManager * sdLogManager)
        : _message { "" },
    _serialPortType { SOFTWARE },
    _serialPortHW { NULL },
    _serialPortSW { serialPortSW },
    _sdLogManager { sdLogManager }  { };

    // //////Non-Variadic/////////////////////////////////////////////
    uint8_t sendData();
    void fastValue(const char fmt, const double value);
    // TODO: err message chanel

    // //////Variadic////////////////////////////////////////////////
    void addValue(const char * fmt, ...);   // variadic
    void fastValue(const char * fmt, ...);  // variadic
    void addValueSC(const char * fmt, ...); // variadic

protected:
    // unsigned char * const _message; // The pointer cant be changed // Descobrir se char nao seria melhor
    String _message = "#";
    uint8_t _serialPortType;
    double _tempValue; // talvez double
    HardwareSerial * _serialPortHW;
    SoftwareSerial * _serialPortSW;
    LogManager * _sdLogManager;
};


#endif // ifndef PRINTMANAGER_H
