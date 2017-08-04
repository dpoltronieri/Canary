#include "Arduino.h"
#include "main.hpp"
#include <../lib/DHT/src/dht.h>
#include <ldr.hpp>
#include <../lib/MQ-Sensor/src/MQSensor.hpp>
#include <../lib/PrintManager/src/PrintManager.hpp>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>


#ifdef RH_PLATFORM_NANO
# include <SoftwareSerial.h>
SoftwareSerial Serial1(10, 11); // RX, TX
#endif

const uint8_t DHT11_PIN = 5;
const uint8_t DHTTYPE   = DHT11;
const uint8_t LDR_PIN   = A0;

// Sensores
DHT dht(DHT11_PIN, DHTTYPE);
ldr LDRSensor = ldr(LDR_PIN);
RtcDS1307<TwoWire> Rtc(Wire);
MQSensor Dummy = MQSensor::NewMQSensor(1, MQ_SENSOR_DUMMY);
// CSPIN inplementado no pino SS, definido pelo sistema
LogManager SD_Loger        = LogManager(SS, VERBOSE, "weater.log");
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
double temporaryData[10];
RtcDateTime clockTime;

// Build
void setup(){
    pinMode(DHT11_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);
    temporaryData[0] = SD_Loger.startLogManager();
    Serial.println(temporaryData[0]);
    SD_Loger.gotoEnd();
    dht.begin();


    Serial.begin(115200);

    // #ifdef RH_PLATFORM_MEGA
    Serial1.begin(115200);
    // #endif

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    Rtc.Begin();

    // TODO: decidir quanto ao settime
    // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // Rtc.SetDateTime(compiled);
    // printDateTime(compiled);
    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    // Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);
} // setup

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
    Print_Manager.addValue("q", temporaryData[4]);

    clockTime         = Rtc.GetDateTime();
    temporaryData[5]  = clockTime.Second();
    temporaryData[6]  = clockTime.Minute();
    temporaryData[7]  = clockTime.Hour();
    temporaryData[8]  = clockTime.Day();
    temporaryData[9]  = clockTime.Month();
    temporaryData[10] = clockTime.Year();
    Print_Manager.addValue("YODHMS",
      temporaryData[10],
      temporaryData[9],
      temporaryData[8],
      temporaryData[7],
      temporaryData[6],
      temporaryData[5]);


    Print_Manager.sendData();

    for (int i = 0; i < 10; i++) {
        Serial.print(temporaryData[i]);
        Serial.print(" , ");
    }
    Serial.println(";");


    delay(2000);
} // loop
