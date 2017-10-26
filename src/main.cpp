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

    // TODO: Colocar essa verificação no setup
    // if (millis() > 5000 && GPS_Module.charsProcessed() < 10) {
    //     Serial.println(F("No GPS detected: check wiring."));
    //     while (true) ;
    // }

    // EEPROM_writeAnything(HOME_LAT, temporaryData[LATITUDE]);
    // EEPROM_writeAnything(HOME_LON, temporaryData[LONGITUDE]);
    EEPROM_readAnything(HOME_LAT, temporaryData[LATITUDE]);
    EEPROM_readAnything(HOME_LON, temporaryData[LONGITUDE]);


    // TODO: decidir quanto ao settime
    // TODO: sincronizar com o GPS_Module
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);
    Serial.println(compiled);
    //  printDateTime(compiled);
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
        temporaryData[TEMPERATURE] = dht.readTemperature();
        temporaryData[HUMIDITY]    = dht.readHumidity();
        // Check if any reads failed and exit early (to try again).
        if (isnan(temporaryData[TEMPERATURE]) || isnan(temporaryData[HUMIDITY])) {
            Serial.println("Failed to read from DHT sensor!");
            Serial1.println("Failed to read from DHT sensor!");
        }
    }
    // TODO: decidir se é um dado relevante
    // float hic = dht.computeHeatIndex(t, h, false);

    temporaryData[LUMINOSITY] = LDRSensor.check();

    Print_Manager.addValue("th", temporaryData[TEMPERATURE], temporaryData[HUMIDITY]);
    Print_Manager.addValue("l", temporaryData[LUMINOSITY]);

    // MQ7_Sensor
    // temporaryData[4] = MQ7_Sensor.check();
    Serial.print("Leitura de monóxido de carbono(tensão, cru): ");
    Serial.print(MQ7_Sensor.MQTension(analogRead(A1)));
    Serial.print("  ");
    Serial.println(analogRead(A1));
    temporaryData[CARBON_MONOXIDE] = MQ7_Sensor.readCarbonMonoxide();
    Print_Manager.addValue("q", temporaryData[CARBON_MONOXIDE]);
    Serial.print("Leitura de monóxido de carbono(PPM): ");
    Serial.println(temporaryData[CARBON_MONOXIDE]);

    Serial.print("Leitura de Alcool(tensão, cru): ");
    Serial.print(MQ3_Sensor.MQTension(analogRead(A1)));
    Serial.print("  ");
    Serial.println(analogRead(A2));
    temporaryData[C2H5OH] = MQ3_Sensor.readC2H5OH();
    Print_Manager.addValue("a", temporaryData[C2H5OH]);
    Serial.print("Leitura de álcool(PPM): ");
    Serial.println(temporaryData[C2H5OH]);

    clockTime = Rtc.GetDateTime();
    // temporaryData[EPOCH32] = clockTime.Epoch32Time();
    // Print_Manager.addValue("g", temporaryData[EPOCH32]);
    size_t tempTime2 = clockTime.Epoch32Time();
    Print_Manager.addValue("g", tempTime2);
    // Mudou para Epoch32

    /*
     * temporaryData[SECOND] = clockTime.Second();
     * temporaryData[MINUTE] = clockTime.Minute();
     * temporaryData[HOUR]   = clockTime.Hour();
     * temporaryData[DAY]    = clockTime.Day();
     * temporaryData[MONTH]  = clockTime.Month();
     * temporaryData[YEAR]   = clockTime.Year();
     * Print_Manager.addValue("YODHMS",
     * temporaryData[YEAR],
     * temporaryData[MONTH],
     * temporaryData[DAY],
     * temporaryData[HOUR],
     * temporaryData[MINUTE],
     * temporaryData[SECOND]);
     * //*/

    // This sketch displays information every time a new sentence is correctly encoded.
    while (Serial2.available() > 0) {
        if (GPS_Module.encode(Serial2.read()))
            displayInfo();
    }

    if (GPS_Module.location.isValid()) {
        temporaryData[LATITUDE]  = GPS_Module.location.lat();
        temporaryData[LONGITUDE] = GPS_Module.location.lng();
        // EEPROM_writeAnything(HOME_LAT, temporaryData[LATITUDE]);
        // EEPROM_writeAnything(HOME_LON, temporaryData[LONGITUDE]);
    }
    Print_Manager.addValue("AO",
      temporaryData[LATITUDE],
      temporaryData[LONGITUDE]);


    Print_Manager.sendData();

    // for (int i = 0; i < 15; i++) {
    //     Serial.print(temporaryData[i]);
    //     Serial.print(" , ");
    // }
    // Serial.println(";");

    delay(2000);
} // loop
