#include <Arduino.h>
#include "PrintManager.hpp"

constexpr int toInt(const char c){
    return (int) c;
}

void PrintManager::addValue(const char * fmt, ...){
    va_list args;

    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == 't') {
            // temperatura
            _tempValue = va_arg(args, double);
            _message  += "+t";
        } else if (*fmt == 'h') {
            // humidade
            // note automatic conversion to integral type
            _tempValue = va_arg(args, double);
            _message  += "+h";
            // _message += c;
        } else if (*fmt == 'l') {
            // sensor MQ
            _tempValue = va_arg(args, double);
            _message  += "+l";
            // _message += d;
        } else if (*fmt == 'M') {
            // sensor MQ
            uint16_t d = va_arg(args, double);
            _message += "+M";
            _message += d;
        } else {
            // return;
            break;
        }
        // */
        _message += _tempValue;
        ++fmt;
    }

    // _message[4] = '\0';

    va_end(args);
};

void PrintManager::fastValue(const char fmt, const double value){
    _message += "+";
    _message += fmt;
    _message.concat(value);
};

uint8_t PrintManager::sendData(){
    if (_serialPortType == HARDWARE) {
        _message += "~";
        _serialPortHW->println(_message);
        Serial.println(_message);
        _message = "#";
        return 0;
    } else {
        return 1;
    }
    return -1;
}

// //////////////////////////////////////////////////////////////////////////

void PrintManager::fastValue(const char * fmt, ...){
    va_list args;

    va_start(args, fmt);

    while (*fmt != '\0') {
        _tempValue = va_arg(args, float);
        _message  += "+";
        char temp = *fmt;
        _message += temp;
        _message.concat(_tempValue);
        ++fmt;
    }

    va_end(args);
};

void PrintManager::addValueSC(const char * fmt, ...){
    va_list args;

    va_start(args, fmt);

    while (*fmt != '\0') {
        // Serial.println("passei 1");
        // int temp = *fmt;
        // Serial.println("passei 2");
        // Serial.println(temp);

        // switch (temp) {
        //     case 't': // Temperature
        //         _tempValue = va_arg(args, float);
        //         _message  += "+t";
        //         Serial.println(*fmt);
        //         break;
        //     case 'h': // Temperature
        //         _tempValue = va_arg(args, float);
        //         _message  += "+h";
        //         break;
        //     case 'l': // Temperature
        //         _tempValue = va_arg(args, float);
        //         _message  += "+l";
        //         break;
        //     case 'M': // Temperature
        //         _tempValue = va_arg(args, float);
        //         _message  += "+M";
        //         break;
        //     default: // TODO: arrumar esse tratamento
        //         break;
        // }


        if (*fmt == 't') {
            // temperatura
            _tempValue = va_arg(args, uint16_t);
            _message  += "+t";
        } else if (*fmt == 'h') {
            // humidade
            // note automatic conversion to integral type
            uint16_t c = va_arg(args, uint16_t);
            _message += "+h";
            _message += c;
        } else if (*fmt == 'l') {
            // sensor MQ
            uint16_t d = va_arg(args, uint16_t);
            _message += "+l";
            _message += d;
        } else if (*fmt == 'M') {
            // sensor MQ
            uint16_t d = va_arg(args, uint16_t);
            _message += "+M";
            _message += d;
        } else {
            // return;
            break;
        }
        // */
        _message += _tempValue;
        ++fmt;
    }

    // _message[4] = '\0';

    va_end(args);
};
