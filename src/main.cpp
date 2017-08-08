#include "Arduino.h"
#include "main.hpp"

/*
 * #include <../lib/DHT/src/dht.h>
 * #include <ldr.hpp>
 * #include <../lib/MQ-Sensor/src/MQSensor.hpp>
 * #include <../lib/PrintManager/src/PrintManager.hpp>
 * #include <Wire.h>
 * #include <RtcDS1307.h>
 * #include <TinyGPS++.h>
 * #include <SPI.h>
 * #include <SD.h>
 * //*/

#ifdef RH_PLATFORM_NANO
# include <SoftwareSerial.h>
SoftwareSerial Serial1(10, 11); // RX, TX
#endif


// Build
void setup(){
    pinMode(52, OUTPUT);
    pinMode(SS, OUTPUT);

    pinMode(DHT11_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);

    dht.begin();


    Serial.begin(115200);

    // #ifdef RH_PLATFORM_MEGA
    Serial1.begin(115200);
    // #endif

    // GPS_Module
    Serial2.begin(9600);


    temporaryData[0] = SD_Loger.startLogManager();
    Serial.println(temporaryData[0]);
    SD_Loger.gotoEnd();

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    Rtc.Begin();

    // EEPROM_writeAnything(HOME_LAT, temporaryData[11]);
    // EEPROM_writeAnything(HOME_LON, temporaryData[12]);
    EEPROM_readAnything(HOME_LAT, temporaryData[11]);
    EEPROM_readAnything(HOME_LON, temporaryData[12]);


    // TODO: decidir quanto ao settime
    // TODO: sincronizar com o GPS_Module
    // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // Rtc.SetDateTime(compiled);
    // printDateTime(compiled);
    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    // Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);
} // setup

void loop(){
    if (GPS_Module.time.isValid() && clockUpdate) {
        clockTime = RtcDateTime(
          GPS_Module.date.year(),
          GPS_Module.date.month(),
          GPS_Module.date.day(),
          GPS_Module.time.hour() - 3,
          GPS_Module.time.minute(),
          GPS_Module.time.second()
          );
        Rtc.SetDateTime(clockTime);
        clockUpdate = false;
    }

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

    temporaryData[3] = LDRSensor.check();

    Print_Manager.addValue("th", temporaryData[1], temporaryData[2]);
    Print_Manager.addValue("l", temporaryData[3]);

    // MQ7_Sensor
    // temporaryData[4] = MQ7_Sensor.check();
    Serial.println(analogRead(A1));
    temporaryData[4] = MQ7_Sensor.readCarbonMonoxide();
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

    // This sketch displays information every time a new sentence is correctly encoded.
    while (Serial2.available() > 0) {
        if (GPS_Module.encode(Serial2.read()))
            displayInfo();
    }

    if (GPS_Module.location.isValid()) {
        temporaryData[11] = GPS_Module.location.lat();
        temporaryData[12] = GPS_Module.location.lng();
        // EEPROM_writeAnything(HOME_LAT, temporaryData[11]);
        // EEPROM_writeAnything(HOME_LON, temporaryData[12]);
    }

    Print_Manager.sendData();

    // for (int i = 0; i < 15; i++) {
    //     Serial.print(temporaryData[i]);
    //     Serial.print(" , ");
    // }
    // Serial.println(";");

    delay(2000);

    // TODO: Colocar essa verificação no setup
    // if (millis() > 5000 && GPS_Module.charsProcessed() < 10) {
    //     Serial.println(F("No GPS detected: check wiring."));
    //     while (true) ;
    // }
} // loop
