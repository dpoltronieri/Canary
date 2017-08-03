#include "Arduino.h"
#include "main.hpp"
#include <../lib/DHT/src/dht.h>
#include <ldr.hpp>
#include <../lib/MQ-Sensor/src/MQSensor.hpp>
#include <../lib/PrintManager/src/PrintManager.hpp>

#ifdef RH_PLATFORM_NANO
# include <SoftwareSerial.h>
SoftwareSerial Serial1(10, 11); // RX, TX
#endif

const uint8_t DHT11_PIN = 5;
const uint8_t DHTTYPE   = DHT11;
const uint8_t LDR_PIN   = A0;

// Sensores
DHT dht(DHT11_PIN, DHTTYPE);
ldr LDRSensor  = ldr(LDR_PIN);
MQSensor Dummy = MQSensor::NewMQSensor(1, MQ_SENSOR_DUMMY);
// CSPIN inplementado no pino SS, definido pelo sistema
LogManager SD_Loger        = LogManager(SS, VERBOSE, "weather.log");
PrintManager Print_Manager = PrintManager(&Serial1, &SD_Loger); // Explicit pointer conversion

/* MONTAGEM
 * LDR/pot pino 4
 * DHT pino 5
 * VAZIO pino 1
 * BLUETOOTH Serial1
 *
 * //*/

// Variáveis de Ambiente
double temporaryData[10];

// Build
void setup(){
    pinMode(DHT11_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);
    SD_Loger.startLogManager();
    SD_Loger.gotoEnd();
    dht.begin();

    Serial.begin(115200);

    // #ifdef RH_PLATFORM_MEGA
    Serial1.begin(115200);
    // #endif
}

void loop(){
    // TODO: reescrever isso

    {
        temporaryData[1] = dht.readTemperature();
        temporaryData[2] = dht.readHumidity();
        // Check if any reads failed and exit early (to try again).
        if (isnan(temporaryData[1]) || isnan(temporaryData[2])) {
            Serial.println("Failed to read from DHT sensor!");
            Serial1.println("Failed to read from DHT sensor!");
        }
    }
    // TODO: decidir se é um dado relevante
    // float hic = dht.computeHeatIndex(t, h, false);

    temporaryData[3] = 1024 - LDRSensor.check();

    Print_Manager.addValue("th", temporaryData[1], temporaryData[2]);
    Print_Manager.addValue("l", temporaryData[3]);

    // Dummy
    temporaryData[4] = (double) Dummy.check();
    Print_Manager.addValue("M", temporaryData[4]);

    Print_Manager.sendData();

    for (int i = 0; i < 10; i++) {
        Serial.print(temporaryData[i]);
        Serial.print(" , ");
    }
    Serial.println(";");

    delay(2000);
} // loop
