/*
 * Created by Daniel P. Poltronieri
 */

#ifndef MQSensor_h
#define MQSensor_h

#include <Arduino.h>

// Enumeration for constructor eficiency
namespace {
enum SENSOR_TYPE : uint8_t {
    MQ_SENSOR_DUMMY,
    MQ_SENSOR_POTENTIOMETER,
    MQ_SENSOR_2,
    MQ_SENSOR_3,
    MQ_SENSOR_4,
    MQ_SENSOR_5,
    MQ_SENSOR_6,
    MQ_SENSOR_7,
    MQ_SENSOR_8,
    MQ_SENSOR_9,
    MQ_SENSOR_135
};
}

class MQSensor {
private:


public:

    static MQSensor NewMQSensor(const uint8_t mqpin, const uint8_t mqtype);
    // This prevents instanciating a LineController and helps with inheritance
    inline uint16_t check(void){
        int16_t temp = analogRead(_mq_pin);

        return temp;
    };

protected:
    MQSensor(const uint8_t mqpin)
        : _mq_pin { mqpin } { }

    uint8_t _mq_pin;
};

class MQDummy : public MQSensor {
public:

    MQDummy (const uint8_t mqpin);

    inline uint16_t check(void){
        _current_value += random(-10, 10);
        if (_current_value < 0)
            _current_value = 0;
        if (_current_value > 255)
            _current_value = 255;
        return _current_value;
    };
protected:
    uint16_t _current_value;
};

class MQPot : public MQSensor {
public:

    MQPot (const uint8_t mqpin);

    inline uint16_t check(void){
        int16_t temp = analogRead(_mq_pin);

        return temp;
    };
protected:
};

#endif // ifndef mq_h
