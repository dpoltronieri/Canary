#include <Arduino.h>
#include "LogManager.hpp"

uint8_t LogManager::startLogManager(){
    if (!SD.begin(_CSpin)) {
        // TODO: atualizar isso para PrintManager
        Serial.println("initialization failed!");
        return 1;
    }
    Serial.println("initialization done.");

    // TODO: decidir se o teste fica
    _LOG = SD.open(_fileName, FILE_WRITE);
    if (_LOG) {
        _LOG.close();
        Serial.println("File OK");
        return 0;
    } else {
        Serial.println("File NOT OK");
        return 1;
    }
}

uint16_t LogManager::println(String message){
    uint16_t returntemp;

    returntemp = _LOG.println(message);
    _LOG.flush();
    if (_VERBOSE)
        Serial.println(message);
    return returntemp;
}

String LogManager::readln(){
    String tempReturn = "";
    char tempCharacter;

    if (_LOG) {
        while (_LOG.available()) {
            tempCharacter = _LOG.read();
            if (tempCharacter == '\n') {
                if (_VERBOSE)
                    Serial.println(tempReturn);
                return String(tempReturn);
            } else {
                tempReturn += tempCharacter;
            }
        }
    }
    if (_VERBOSE)
        Serial.println(tempReturn);
    return tempReturn;
}

void LogManager::gotoTop(){
    _LOG.flush();
    _LOG.close();
    _LOG = SD.open(_fileName, FILE_WRITE);
}

void LogManager::gotoEnd(){
    while (_LOG.available()) {
        _LOG.read();
    }
}

void LogManager::flush(){
    _LOG.flush();
}

void LogManager::close(){
    _LOG.close();
}
