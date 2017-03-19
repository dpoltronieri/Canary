#include "Arduino.h"
#include "main.hpp"
#include "dht.h"
#include <ldr.hpp>
#include <MQSensor.hpp>

#ifdef RH_PLATFORM_NANO
# include <SoftwareSerial.h>
SoftwareSerial Serial1(10, 11); // RX, TX
#endif

#define DHT11_PIN 5
#define LDR_PIN   4

// Sensores
dht DHTSensor;
ldr LDRSensor = ldr(LDR_PIN);

// Variáveis de Ambiente
uint16_t TemperatureHumidity[4];


void setup(){
    Serial.begin(115200);
    // Por enquanto
    Serial.println("DHT TEST PROGRAM ");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT_LIB_VERSION);
    Serial.println();
    Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");

    #ifdef RH_PLATFORM_MEGA
    Serial1.begin(115200);
    #endif
}

void loop(){
    dhtDebug(DHTSensor, DHT11_PIN);
    TemperatureHumidity[0] = DHTSensor.read11(DHT11_PIN);
    TemperatureHumidity[1] = DHTSensor.temperature;
    TemperatureHumidity[2] = DHTSensor.humidity;
    TemperatureHumidity[3] = LDRSensor.check();


    delay(2000);
} // loop
