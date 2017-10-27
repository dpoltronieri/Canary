#include <Arduino.h>
#include "MQSensor.hpp"

/*
 * https://www.mycurvefit.com/
 */

/**
 * Re-maps a number from one range to another. That is, a value of in_min would
 * get mapped to out_min, a value of in_max to out_max, values in-between to
 * values in-between, etc.
 * @param  x
 * The value to be mapped.
 * @param  in_min
 * The lower bond of the value's current range.
 * @param  in_max
 * The upper bond of the value's current range.
 * @param  out_min
 * The lower bond of the value's target range.
 * @param  out_max
 * The upper bond of the value's target range.
 * @return
 * The mapped value.
 */
template <typename T>
T map(T x, T in_min, T in_max, T out_min, T out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ Sensor ///////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////


/**
 * MQSensor::NewMQSensor creates a MQSensor object of the correct sensor type.
 * This function is to de used to create a MQSensor array of different sensor types.
 * @param  mqpin
 * The arduino pin to which the sensor is connected.
 * @param  mqtype
 * The type of the sensor, utilizing the SENSOR_TYPE enumeration.
 * @return
 * An instance of the chosen sensor.
 */
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

/**
 * The constructor is protected, making it impossible to instanciate a MQSensor object.
 * This constructor is internally utilized as a part of the sons constructors.
 * @param mqpin
 * The arduino pin to which the sensor is connected.
 */
MQSensor::MQSensor(const uint8_t mqpin)
    : _MQ_pin {
    mqpin
} {
    pinMode(_MQ_pin, INPUT);
}

/**
 * MQSensor::_Ro setter.
 */
void MQSensor::SetRo(const float ro_factor){
    _Ro = ro_factor;
};

/**
 * MQSensor::_RO_CLEAN_AIR_FACTOR setter.
 */
void MQSensor::SetRoCleanAirFactor(const float ro_clean_air_factor){
    _RO_CLEAN_AIR_FACTOR = ro_clean_air_factor;
};

/**
 * MQSensor::_RL_VALUE setter.
 */
void MQSensor::setRlValue(const uint8_t rlvalue){
    _RL_VALUE = rlvalue;
};

/**
 * MQSensor::_CALIBARAION_SAMPLE_TIMES setter.
 */
void MQSensor::setCalibrationSampleTimes(const uint8_t cst){
    _CALIBARAION_SAMPLE_TIMES = cst;
};

/**
 * MQSensor::_CALIBRATION_SAMPLE_INTERVAL setter.
 */
void MQSensor::setCalibrationSampleInterval(const uint8_t csi){
    _CALIBRATION_SAMPLE_INTERVAL = csi;
};

/**
 * MQSensor::_READ_SAMPLE_INTERVAL setter.
 */
void MQSensor::setReadSampleInterval(const uint8_t rsi){
    _READ_SAMPLE_INTERVAL = rsi;
};

/**
 * MQSensor::_READ_SAMPLE_TIMES setter.
 */
void MQSensor::setReadSampleTimes(const uint8_t rst){
    _READ_SAMPLE_TIMES = rst;
};

/**
 * MQSensor::_RO_CLEAN_AIR_FACTOR getter.
 */
float const MQSensor::GetRoCleanAirFactor(void){
    return _RO_CLEAN_AIR_FACTOR;
};

/**
 * MQSensor::_Ro getter.
 */
float const MQSensor::GetRo(void){
    return _Ro;
};

/**
 * Returns the voltage either in the load resistor, or in the MQ sensor,
 * either one that's between the MQSensor::_MQ_pin and the ground connection.
 *
 * This is usually used as a part of the MQSensor::MQGetPPM function.
 * @param  raw_adc
 * The ADC value of the voltage divider on the MQSensor::_MQ_pin.
 * @return
 * The tension value remapped to a float value between 0 and 5 volts.
 */
float MQSensor::MQTension(const float raw_adc){
    return map(raw_adc, 0.0f, 1023.0f, 0.0f, 5.0f);
}

/**
 * Returns the tension on the sensor, solving the voltage divider for R1 as seen
 * on https://en.wikipedia.org/wiki/Voltage_divider. The tension is given by a
 * voltage divider, which has its load resistor set in _RL_VALUE, and can be
 * adjusted in code if the sensor comes before or after the load resistor.
 * @param  raw_adc The ADC value of the voltage divider on the _MQ_pin.
 * @return The resistance of the MQ sensor.
 */
float const inline MQSensor::MQResistanceCalculation(float raw_adc){
    float adc_tension = MQTension(raw_adc);

    // Se for ficar depois na série
    // float sensor_resistance = _RL_VALUE / ( (5.0f / adc_tension) - 1);

    // Se for ficar antes na série
    float sensor_resistance = _RL_VALUE * ( (5.0f / adc_tension) - 1);

    return sensor_resistance;
}

/**
 * Reads the resistance value os the sensor MQSensor::_CALIBARAION_SAMPLE_TIMES
 * with a delay pf MQSensor::_CALIBRATION_SAMPLE_INTERVAL between readings and
 * uses the average value os the readings to set the temporary RO, witch is
 * divided by MQSensor::_RO_CLEAN_AIR_FACTOR to yeld the current RO.
 * @return The current RO.
 */
float MQSensor::MQCalibration(){
    float _temp_ro = 0;

    for (int i = 0; i < _CALIBARAION_SAMPLE_TIMES; i++) {
        _temp_ro += MQResistanceCalculation(analogRead(_MQ_pin));
        delay(_CALIBRATION_SAMPLE_INTERVAL);
    }
    _temp_ro = _temp_ro / _CALIBARAION_SAMPLE_TIMES;

    _temp_ro = _temp_ro / _RO_CLEAN_AIR_FACTOR;
    // divided by RO_CLEAN_AIR_FACTOR yields the Ro
    // according to the chart in the datasheet
    return _temp_ro;
}

/**
 * Uses the MQSensor::MQCalibration() to directly ser the _RO_CLEAN_AIR_FACTOR.
 * @return
 * The current _RO_CLEAN_AIR_FACTOR.
 */
float MQSensor::cleanAirCallibrate(){
    _RO_CLEAN_AIR_FACTOR = MQCalibration();
    return _RO_CLEAN_AIR_FACTOR;
}

/**
 * Reads the MQSensor::_MQ_pin port MQSensor::_READ_SAMPLE_TIMES times with a
 * delay of MQSensor::_READ_SAMPLE_INTERVAL between the reading to give a
 * stabilized output.
 * @return The averave readings of the MQSensor::_MQ_pin port.
 */
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

/**
 * Uses the gas curve declared in each MQSensor class to calculate the current
 * gas concentration in parts per million.
 * @param  sensor_tension T
 * he current tension, usually measured with the MQSensor::MQTension() method.
 * @param  gas_curve
 * The A and B parameters of the exponential regression of the gas concentration curve.
 * @return The current gas concentration.
 */
float const MQSensor::MQGetPPM(const float sensor_tension, const float * gas_curve){
    /**
     * Solves y = AB^x to obtain the current value.
     */
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

/**
 * Contructor, creates a MQ3 object.
 * @param mqpin
 * The arduino pin to which the sensor is connected.
 */
MQ3::MQ3(const uint8_t mqpin) : MQSensor(mqpin){ };

/**
 * Checks if the current data is old, then proceeds to either checking the
 * sensor for the current value or returns the stored value.
 * @return The most recent C2H5OH data point.
 */
float MQ3::readC2H5OH(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _C2H5OH != 0) {
        return _C2H5OH;
    } else {
        // _C2H5OH = MQGetGasPercentage(MQRead() / _Ro, GAS_C2H5OH);
        _C2H5OH = MQGetPPM(MQTension(MQRead()), _C2H5OHCurve);
        return _C2H5OH;
    }
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ 7 ////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

/**
 * MQ7::MQ7 constructor.
 * @param mqpin
 * The arduino pin to which the sensor is connected.
 */
MQ7::MQ7(const uint8_t mqpin) : MQSensor(mqpin){
    // pinMode(_MQ_pin, INPUT);
};

// Conjunto de funções de fácil legibilidade

/**
 * Checks if the current data is old, then proceeds to either checking the
 * sensor for the current value or returns the stored value.
 * @return The most recent H2 data point.
 */
float MQ7::readH2(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _H2 != 0) {
        return _H2;
    } else {
        _H2 = MQGetPPM(MQTension(MQRead()), _H2Curve);
        return _H2;
    }
}

/**
 * Checks if the current data is old, then proceeds to either checking the
 * sensor for the current value or returns the stored value.
 * @return The most recent Carbon Monoxide data point.
 */
float MQ7::readCarbonMonoxide(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _CarbonMonoxide != 0) {
        return _CarbonMonoxide;
    } else {
        _CarbonMonoxide = MQGetPPM(MQTension(MQRead()), _CarbonMonoxideCurve);
        return _CarbonMonoxide;
    }
}
