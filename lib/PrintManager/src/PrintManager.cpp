#include <Arduino.h>
#include "PrintManager.hpp"

void PrintManager::addValue(const char * fmt, ...){
    va_list args;

    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == 't') {
            // temperatura
            uint16_t i = va_arg(args, uint16_t);
            _message[0] = 't';
            _message[1] = i;
            // std::cout << i << '\n';
        } else if (*fmt == 'h') {
            // humidade
            // note automatic conversion to integral type
            uint16_t c = va_arg(args, uint16_t);
            _message[2] = 'h';
            _message[3] = c;
            // std::cout << static_cast<char>(c) << '\n';
        } else if (*fmt == 'M') {
            // sensor MQ
            uint16_t d = va_arg(args, uint16_t);
            _message[2] = 'M';
            _message[3] = d;
            // std::cout << d << '\n';
        }
        ++fmt;
    }

    _message[4] = '\0';

    va_end(args);
};

uint8_t PrintManager::sendData(){
    if (_serialPortType == HARDWARE) {
        for (size_t i = 0; _message[i] != '\0'; i++)
            _serialPortHW->write(_message[i]);
        _serialPortHW->write('\0');
        return 0;
    } else {
        return 1;
    }
    return -1;
}
