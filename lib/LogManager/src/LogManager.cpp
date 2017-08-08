#include <Arduino.h>
#include "LogManager.hpp"

uint8_t LogManager::startLogManager(){
    if (!SD.begin(_CSpin, SPI_HALF_SPEED)) {
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
        _LOG.close();
        Serial.println("File NOT OK");
        return 2;
    }
} // startLogManager

/*
 * uint16_t LogManager::println(String message){
 *  uint16_t returntemp;
 *
 *  returntemp = _LOG.println(message);
 *  _LOG.flush();
 *  if (_VERBOSE)
 *      Serial.println(message + " para o SD " + returntemp);
 *  return returntemp;
 * }
 * //*/
uint16_t LogManager::println(const String message){
    uint16_t temp;

    if (SD.exists(_fileName)) {
        // check the card is still there
        // now append new data file
        _LOG = SD.open(_fileName, FILE_WRITE);
        if (_LOG) {
            temp = _LOG.println(message);
            _LOG.close(); // close the file
            // Serial.println(temp);
        }
    } else {
        Serial.println("Error writing to file !");
    }
    return temp;
};

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
