#ifndef MAIN_H
#define  MAIN_H
#include "Arduino.h"
#include "dht.h"

// Variáveis de ambiente

void dhtDebug(dht Sensor, uint8_t DHT11_PIN){
	Serial.print("DHT11, \t");
	int chk = Sensor.read11(DHT11_PIN);
	switch (chk) {
	case DHTLIB_OK:
		Serial.print("OK,\t");
		break;
	case DHTLIB_ERROR_CHECKSUM:
		Serial.print("Checksum error,\t");
		break;
	case DHTLIB_ERROR_TIMEOUT:
		Serial.print("Time out error,\t");
		break;
	case DHTLIB_ERROR_CONNECT:
		Serial.print("Connect error,\t");
		break;
	case DHTLIB_ERROR_ACK_L:
		Serial.print("Ack Low error,\t");
		break;
	case DHTLIB_ERROR_ACK_H:
		Serial.print("Ack High error,\t");
		break;
	default:
		Serial.print("Unknown error,\t");
		break;
	}

	Serial.print(Sensor.humidity, 1);
	Serial.print(",\t");
	Serial.println(Sensor.temperature, 1);
}

#endif // ifndef MAIN_H
