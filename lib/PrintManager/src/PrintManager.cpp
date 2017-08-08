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
        } else if (*fmt == 'q') {
            // sensor MQ
            _tempValue = va_arg(args, double);
            _message  += "+q";
            // _message += d;

            /*
             * Date and Time Varibles
             */
        } else if (*fmt == 'S') {
            _tempValue = va_arg(args, double);
            _message  += "+S";
        } else if (*fmt == 'M') {
            _tempValue = va_arg(args, double);
            _message  += "+M";
        } else if (*fmt == 'H') {
            _tempValue = va_arg(args, double);
            _message  += "+H";
        } else if (*fmt == 'D') {
            _tempValue = va_arg(args, double);
            _message  += "+D";
        } else if (*fmt == 'O') {
            _tempValue = va_arg(args, double);
            _message  += "+O";
        } else if (*fmt == 'Y') {
            _tempValue = va_arg(args, double);
            _message  += "+Y";
        } else if (*fmt == 'A') {
            _tempValue = va_arg(args, double);
            _message  += "+A";
        } else if (*fmt == 'O') {
            _tempValue = va_arg(args, double);
            _message  += "+O";

            /*
             * Date and Time Varibles End
             */
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
    _message += "~";
    if (_sdLogManager) {
        _sdLogManager->println(_message);
    }
    if (_serialPortType == HARDWARE) {
        _serialPortHW->println(_message);
    } else if (_serialPortType == SOFTWARE) {
        _serialPortSW->println(_message);
    } else {
        return 1;
    }
    Serial.println(_message);
    _message = "#";
    return 0;
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
        //     case 'q': // Temperature
        //         _tempValue = va_arg(args, float);
        //         _message  += "+q";
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
        } else if (*fmt == 'q') {
            // sensor MQ
            uint16_t d = va_arg(args, uint16_t);
            _message += "+q";
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
