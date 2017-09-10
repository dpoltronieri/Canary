#include <Arduino.h>
#include "MQSensor.hpp"

/*
 * https://www.mycurvefit.com/
 */

// TODO: mudar isso para template
float fmap(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ Sensor ///////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

/*****************************************************************************
* NewMQSensor is a function that returns a constructed object MQSensor of the coosen type.
*****************************************************************************/
MQSensor MQSensor::NewMQSensor(const uint8_t mqpin, const uint8_t mqtype){
    switch (mqtype) {
        case MQ_SENSOR_DUMMY:
            return MQDummy(mqpin); // Observar se não precisa do new

            break;

        case MQ_SENSOR_POTENTIOMETER:
            return MQPotentiometer(mqpin);

            break;

        case MQ_SENSOR_7:
            return MQ7(mqpin);

            break;

        default:
            return 0;
    }
}

MQSensor::MQSensor(const uint8_t mqpin)
    : _MQ_pin {
    mqpin
} {
    pinMode(_MQ_pin, INPUT);
}

void MQSensor::SetRo(const float ro_factor){
    _Ro = ro_factor;
};
void MQSensor::SetRoCleanAirFactor(const float ro_clean_air_factor){
    _RO_CLEAN_AIR_FACTOR = ro_clean_air_factor;
};
void MQSensor::setRlValue(const uint8_t rlvalue){
    _RL_VALUE = rlvalue;
};
void MQSensor::setCalibrationSampleTimes(const uint8_t cst){
    _CALIBARAION_SAMPLE_TIMES = cst;
};
void MQSensor::setCalibrationSampleInterval(const uint8_t csi){
    _CALIBRATION_SAMPLE_INTERVAL = csi;
};
void MQSensor::setReadSampleInterval(const uint8_t rsi){
    _READ_SAMPLE_INTERVAL = rsi;
};
void MQSensor::setReadSampleTimes(const uint8_t rst){
    _READ_SAMPLE_TIMES = rst;
};
float const MQSensor::GetRoCleanAirFactor(void){
    return _RO_CLEAN_AIR_FACTOR;
};
float const MQSensor::GetRo(void){
    return _Ro;
};

float MQSensor::MQTension(const float raw_adc){
    return fmap(raw_adc, 0, 1023, 0.0, 5.0);
}

/****************** MQResistanceCalculation ****************************************
 * Input:   raw_adc - raw value read from adc, which represents the voltage
 * Output:  the calculated sensor resistance
 * Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
 *       across the load resistor and its resistance, the resistance of the sensor
 *       could be derived.
 ************************************************************************************/
// utilizando divisor de tensão e resolvendo para R1
float const inline MQSensor::MQResistanceCalculation(float raw_adc){
    // https://en.wikipedia.org/wiki/Voltage_divider
    float adc_tension = MQTension(raw_adc);

    // Se for ficar depois na série
    // float sensor_resistance = _RL_VALUE / ( (5.0f / adc_tension) - 1);

    // Se for ficar antes na série
    float sensor_resistance = _RL_VALUE * ( (5.0f / adc_tension) - 1);

    return sensor_resistance;
}

/***************************** MQCalibration ****************************************
*  Input:   mq_pin - analog channel
*  Output:  Ro of the sensor
*  Remarks: This function assumes that the sensor is in clean air. It use
*        MQResistanceCalculation to calculates the sensor resistance in clean air
*        and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about
*        10, which differs slightly between different sensors.
************************************************************************************/
float MQSensor::MQCalibration(){
    float _temp_ro = 0;

    for (int i = 0; i < _CALIBARAION_SAMPLE_TIMES; i++) {
        // take multiple samples
        _temp_ro += MQResistanceCalculation(analogRead(_MQ_pin));
        delay(_CALIBRATION_SAMPLE_INTERVAL);
    }
    _temp_ro = _temp_ro / _CALIBARAION_SAMPLE_TIMES; // calculate the average value

    _temp_ro = _temp_ro / _RO_CLEAN_AIR_FACTOR; // divided by RO_CLEAN_AIR_FACTOR yields the Ro
    // according to the chart in the datasheet
    return _temp_ro;
}

float MQSensor::cleanAirCallibrate(){
    _RO_CLEAN_AIR_FACTOR = MQCalibration();
    return _RO_CLEAN_AIR_FACTOR;
}

/***************************** MQRead ****************************************
 *  Input:   _MQ_pin - analog channel
 *  Output:  A stabilized analogRead
 *  Remarks: This function returns the average value read _READ_SAMPLE_TIMES times
 * with a delay of _READ_SAMPLE_INTERVAL.
 ************************************************************************************/
float const MQSensor::MQRead(){
    float _read_sample = 0;
    float _temp_resistance;

    for (int i = 0; i < _READ_SAMPLE_TIMES; i++) {
        _temp_resistance = analogRead(_MQ_pin);
        _read_sample    += MQResistanceCalculation(_temp_resistance);
        delay(_READ_SAMPLE_INTERVAL);
    }

    _read_sample = _read_sample / _READ_SAMPLE_TIMES;
    return _read_sample;
}

/*****************************  MQGetPPM **********************************
 * Input:   rs_ro_ratio - Rs divided by Ro
 *       pcurve      - pointer to the curve of the target gas
 * Output:  ppm of the target gas
 * Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm)
 *       of the line could be derived if y(rs_ro_ratio) is provided. As it is a
 *       logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic
 *       value.
 ************************************************************************************/
float const MQSensor::MQGetPPM(const float sensor_tension, const float * gas_curve){
    // Resolvendo y = AB^x
    float temp = (float) gas_curve[0] * pow(gas_curve[1], sensor_tension);

    return temp;
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ Dummy ////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

MQDummy::MQDummy(const uint8_t mqpin) : MQSensor(mqpin){
    // pinMode(_MQ_pin, INPUT);
    _current_value = 150;
};

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ Potentiometer ////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

MQPotentiometer::MQPotentiometer (const uint8_t mqpin) : MQSensor(mqpin){
    // pinMode(_MQ_pin, INPUT);
};


// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ 3 ////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

MQ3::MQ3(const uint8_t mqpin) : MQSensor(mqpin){
    // pinMode(_MQ_pin, INPUT);
};

// Conjunto de funções de fácil legibilidade
float MQ3::readC2H5OH(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _C2H5OH != 0) {
        return _C2H5OH;
    } else {
        // return _C2H5OH = MQGetGasPercentage(MQRead() / _Ro, GAS_C2H5OH);
        // One less function call
        return MQGetPPM(MQTension(MQRead()), _C2H5OHCurve);
    }
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ 7 ////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

MQ7::MQ7(const uint8_t mqpin) : MQSensor(mqpin){
    // pinMode(_MQ_pin, INPUT);
};

// Conjunto de funções de fácil legibilidade
float MQ7::readH2(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _H2 != 0) {
        return _H2;
    } else {
        _H2 = MQGetPPM(MQTension(MQRead()), _H2Curve);
        return _H2;
    }
}

float MQ7::readCarbonMonoxide(){
    // TODO: verificaćão de temperatura
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _CarbonMonoxide != 0) {
        return _CarbonMonoxide;
    } else {
        _CarbonMonoxide = MQGetPPM(MQTension(MQRead()), _CarbonMonoxideCurve);
        return _CarbonMonoxide;
    }
}
