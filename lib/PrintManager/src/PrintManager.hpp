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
        : _serialPortType { HARDWARE },
    _serialPortHW { serialPortHW },
    _serialPortSW { NULL },
    _sdLogManager { NULL } { };

    PrintManager(SoftwareSerial * serialPortSW)
        : _serialPortType { SOFTWARE },
    _serialPortHW { NULL },
    _serialPortSW { serialPortSW },
    _sdLogManager { NULL }  { };

    PrintManager(HardwareSerial * serialPortHW, LogManager * sdLogManager)
        : _serialPortType { HARDWARE },
    _serialPortHW { serialPortHW },
    _serialPortSW { NULL },
    _sdLogManager { sdLogManager } { };

    PrintManager(SoftwareSerial * serialPortSW, LogManager * sdLogManager)
        : _serialPortType { SOFTWARE },
    _serialPortHW { NULL },
    _serialPortSW { serialPortSW },
    _sdLogManager { sdLogManager }  { };

    // //////Non-Variadic/////////////////////////////////////////////
    uint8_t sendData();
    void fastValue(const char fmt, const double value);
    // TODO: err message chanel

    // //////Variadic////////////////////////////////////////////////
    void addValue(const char * fmt, ...);  // variadic
    void addHeader(const char * fmt, ...); // variadic
    void fastValue(const char * fmt, ...); // variadic

protected:
    String _messageHeader = "{",
      _messageCurrently   = "\"currently\": {",
      _timezone = "\"Brazil/Brasília\"";
    uint8_t _serialPortType;
    double _tempValue, _latitude, _longitude; // talvez double
    HardwareSerial * _serialPortHW;
    SoftwareSerial * _serialPortSW;
    LogManager * _sdLogManager;

    void makeHeader();
    void makeCurrently();
};


#endif // ifndef PRINTMANAGER_H
