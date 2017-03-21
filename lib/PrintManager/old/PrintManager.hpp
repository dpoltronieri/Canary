#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <Arduino.h>
// #include <SoftwareSerial.h>

class PrintManager {
public:
    PrintManager(HardwareSerial & serialPortHW) : _serialPortHW { serialPortHW } { };
    // PrintManager(SoftwareSerial &serialPortSW) : _serialPortSW(serialPortSW){ };
protected:
    HardwareSerial & _serialPortHW;
    // SoftwareSerial _serialPortSW;
};


#endif // ifndef PRINTMANAGER_H
