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
            _tempValue         = va_arg(args, double);
            _messageCurrently += "\"temperature\": ";
        } else if (*fmt == 'h') {
            // humidade
            // note automatic conversion to integral type
            _tempValue         = va_arg(args, double);
            _messageCurrently += "\"humidity\": ";
        } else if (*fmt == 'l') {
            // sensor MQ
            _tempValue         = va_arg(args, double);
            _messageCurrently += "\"luminosity\": ";
        } else if (*fmt == 'q') {
            // sensor MQ
            _tempValue         = va_arg(args, double);
            _messageCurrently += "\"carbon monoxide\": ";
        } else if (*fmt == 'a') {
            // TODO: mover todos os MQ para um case
            _tempValue         = va_arg(args, double);
            _messageCurrently += "\"C2H5OH\": ";


            /*
             * Date and Time Varibles
             */
        } else if (*fmt == 'g') {
            // TODO: Voltar aqui e retornar direto de size_t ao invés de trensformar em double
            size_t tempTime = va_arg(args, size_t);
            _tempValue = tempTime;
            Serial.println("Time:");
            Serial.println(tempTime);
            Serial.println(_tempValue);
            _messageCurrently += "\"time\": ";

            /*
             * } else if (*fmt == 'S') {
             * _tempValue = va_arg(args, double);
             * _message  += "+S";
             * } else if (*fmt == 'M') {
             * _tempValue = va_arg(args, double);
             * _message  += "+M";
             * } else if (*fmt == 'H') {
             * _tempValue = va_arg(args, double);
             * _message  += "+H";
             * } else if (*fmt == 'D') {
             * _tempValue = va_arg(args, double);
             * _message  += "+D";
             * } else if (*fmt == 'O') {
             * _tempValue = va_arg(args, double);
             * _message  += "+O";
             * } else if (*fmt == 'Y') {
             * _tempValue = va_arg(args, double);
             * _message  += "+Y";
             * // Latitude e longitude trabalham diferente
             * } else if (*fmt == 'A') {
             * _tempValue = va_arg(args, double);
             * _message  += "+A";
             * } else if (*fmt == 'O') {
             * _tempValue = va_arg(args, double);
             * _message  += "+O";
             * //*/

            /*
             * Date and Time Varibles End
             */
        } else {
            // return;
            break;
        }
        // */
        _messageCurrently += _tempValue;
        _messageCurrently += ", ";
        ++fmt;
    }

    // _message[4] = '\0';

    va_end(args);
};

void PrintManager::addHeader(const char * fmt, ...){
    va_list args;

    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == 'A') {
            // Latutude
            _latitude = va_arg(args, double);
        } else if (*fmt == 'O') {
            // Longitude
            _longitude = va_arg(args, double);
        } else {
            break;
        }
        ++fmt;
    }

    va_end(args);
};

void PrintManager::makeHeader(void){
    _messageHeader  = "{\"latitude\": ";
    _messageHeader += _latitude;
    _messageHeader += ",\"longitude\": ";
    _messageHeader += _longitude;
    _messageHeader += ",\"timezone\": ";
    _messageHeader += _timezone;
    _messageHeader += ", ";
}

void PrintManager::makeCurrently(void){
    _messageCurrently.remove(_messageCurrently.length() - 2);
    _messageCurrently += "}}";
}

void PrintManager::fastValue(const char fmt, const double value){
    // _message += "+";
    // _message += fmt;
    // _message.concat(value);
};

uint8_t PrintManager::sendData(){
    // _message += "~";
    makeHeader();
    makeCurrently();
    if (_sdLogManager) {
        _sdLogManager->println(_messageHeader + _messageCurrently);
    }
    if (_serialPortType == HARDWARE) {
        _serialPortHW->println(_messageHeader + _messageCurrently);
    } else if (_serialPortType == SOFTWARE) {
        _serialPortSW->println(_messageHeader + _messageCurrently);
    } else {
        return 1;
    }
    Serial.println(_messageHeader + _messageCurrently);
    _messageCurrently = "\"currently\": {";
    return 0;
}
