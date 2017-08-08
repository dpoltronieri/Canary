#ifndef MAIN_H
#define  MAIN_H
#include "Arduino.h"
#include <../lib/DHT/src/dht.h>
#include <ldr.hpp>
#include <../lib/MQ-Sensor/src/MQSensor.hpp>
#include <../lib/PrintManager/src/PrintManager.hpp>
#include <Wire.h>
#include <RtcDS1307.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

const uint8_t DHT11_PIN = 5;
const uint8_t DHTTYPE   = DHT11;
const uint8_t LDR_PIN   = A0;
const uint8_t HOME_LAT  = 0 * sizeof(double), HOME_LON = 1 * sizeof(double);

// TODO: utilizar isso para definir temporaryData
namespace {
enum dataLocation : const uint8_t {
    TEMPERATURE = 1,
    HUMIDITY,
    LUMINOSITY,
    MQMQ7_Sensor,
    YEAR,
    DAY,
    HOUR,
    MINUTE,
    SECOND,
    DATA_SIZE
};
}

// Sensores
DHT dht(DHT11_PIN, DHTTYPE);
ldr LDRSensor = ldr(LDR_PIN);
RtcDS1307<TwoWire> Rtc(Wire);
TinyGPSPlus GPS_Module;
// MQSensor MQ7_Sensor = MQSensor::NewMQSensor(A1, MQ_SENSOR_7);
MQ7 MQ7_Sensor = MQ7(A1);
// CSPIN inplementado no pino SS, definido pelo sistema
LogManager SD_Loger        = LogManager(52, VERBOSE, "weater.log");
PrintManager Print_Manager = PrintManager(&Serial1, &SD_Loger); // Explicit pointer conversion

/* MONTAGEM
 * LDR/pot pino 4
 * DHT pino 5
 * VAZIO pino 1
 * BLUETOOTH Serial1
 *
 * //*/

// Variáveis de Ambiente
// TODO: Considerar std::vector<int> array
double temporaryData[15];
RtcDateTime clockTime;
bool clockUpdate = true;
File myFile;

void displayInfo(){
    Serial.print(F("Location: "));
    if (GPS_Module.location.isValid()) {
        Serial.print(GPS_Module.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(GPS_Module.location.lng(), 6);
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (GPS_Module.date.isValid()) {
        Serial.print(GPS_Module.date.month());
        Serial.print(F("/"));
        Serial.print(GPS_Module.date.day());
        Serial.print(F("/"));
        Serial.print(GPS_Module.date.year());
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (GPS_Module.time.isValid()) {
        if (GPS_Module.time.hour() < 10) Serial.print(F("0"));
        Serial.print(GPS_Module.time.hour());
        Serial.print(F(":"));
        if (GPS_Module.time.minute() < 10) Serial.print(F("0"));
        Serial.print(GPS_Module.time.minute());
        Serial.print(F(":"));
        if (GPS_Module.time.second() < 10) Serial.print(F("0"));
        Serial.print(GPS_Module.time.second());
        Serial.print(F("."));
        if (GPS_Module.time.centisecond() < 10) Serial.print(F("0"));
        Serial.print(GPS_Module.time.centisecond());
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.println();
} // displayInfo

template <class T> int EEPROM_writeAnything(int ee, const T& value){
    const byte * p = (const byte *) (const void *) &value;
    int i;

    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value){
    byte * p = (byte *) (void *) &value;
    int i;

    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
}

#endif // ifndef MAIN_H
