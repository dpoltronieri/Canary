#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

namespace {
enum verboseType : const uint8_t {
    QUIET   = 0,
    VERBOSE = 1
};
}

class LogManager {
public:

    /*
     * The communication between the microcontroller and the SD card uses SPI,
     * which takes place on digital pins 11, 12, and 13 (on most Arduino boards)
     * or 50, 51, and 52 (Arduino Mega). Additionally, another pin must be used
     * to select the SD card. This can be the hardware SS pin - pin 10 (on most
     * Arduino boards) or pin 53 (on the Mega) - or another pin specified in the
     * call to SD.begin()
     */
    // cspin normalmente é 4
    LogManager(uint8_t cspin, uint8_t verbose = QUIET, const char * fileName = "WEATHER.txt")
        : _CSpin { cspin },
    _VERBOSE { verbose },
    _fileName { fileName } { };

    // Funtion called in the Setup()
    uint8_t startLogManager();
    uint16_t println(String message);
    String readln();
    void gotoTop();
    void gotoEnd();
    void flush();
    void close();

protected:
    uint8_t _CSpin, _VERBOSE;
    File _LOG;
    String _message;
    const char * _fileName;
};


#endif // ifndef LOGMANAGER_H
