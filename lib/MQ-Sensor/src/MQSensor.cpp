#include <Arduino.h>
#include "MQSensor.hpp"

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ Sensor ///////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////
MQSensor MQSensor::NewMQSensor(const uint8_t mqpin, const uint8_t mqtype){
    switch (mqtype) {
        case MQ_SENSOR_DUMMY:
            return MQDummy(mqpin); // Observar se não precisa do new

            break;

        case MQ_SENSOR_POTENTIOMETER:
            return MQPotentiometer(mqpin);

            break;

        case MQ_SENSOR_2:
            return MQ2(mqpin);

            break;

        default:
            return 0;
    }
}

MQSensor::MQSensor(const uint8_t mqpin)
    : _MQ_pin { mqpin } {
    pinMode(_MQ_pin, INPUT);
}

/*
 * MQSensor::MQSensor(const uint8_t mqpin, const float ro_clean_air_factor, const float ro_factor)
 *  : _MQ_pin { mqpin },
 * _RO_CLEAN_AIR_FACTOR {
 *  ro_clean_air_factor
 * },
 * _Ro {
 *  ro_factor
 * } {
 *  pinMode(_MQ_pin, INPUT);
 * }
 * //*/
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

// TODO: verificar se isso é uma bia idéia
void MQSensor::begin(){
    _Ro = MQCalibration();
    if (Serial.available()) {
        Serial.print("Ro: ");
        Serial.print(_Ro);
        Serial.println(" kohm");
    }
}

/****************** MQResistanceCalculation ****************************************
 * Input:   raw_adc - raw value read from adc, which represents the voltage
 * Output:  the calculated sensor resistance
 * Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
 *       across the load resistor and its resistance, the resistance of the sensor
 *       could be derived.
 ************************************************************************************/
// utilizando divisor de tensão e resolvendo para R1
float const inline MQSensor::MQResistanceCalculation(int raw_adc){
    return (((float) _RL_VALUE * (1023 - raw_adc) / raw_adc));
}

/***************************** MQCalibration ****************************************
*  Input:   mq_pin - analog channel
*  Output:  Ro of the sensor
*  Remarks: This function assumes that the sensor is in clean air. It use
*        MQResistanceCalculation to calculates the sensor resistance in clean air
*        and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about
*        10, which differs slightly between different sensors.
************************************************************************************/

/*
 * float MQCalibration(int mq_pin)
 * {
 * int i;
 * float val=0;
 *
 * for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
 *  val += MQResistanceCalculation(analogRead(mq_pin));
 *  delay(CALIBRATION_SAMPLE_INTERVAL);
 * }
 * val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average value
 *
 * val = val/RO_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro
 *                                                      //according to the chart in the datasheet
 *
 * return val;
 * }
 * //*/
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

float const MQSensor::MQRead(){
    float _read_sample = 0;
    int _temp_resistance;

    for (int i = 0; i < _READ_SAMPLE_TIMES; i++) {
        _temp_resistance = analogRead(_MQ_pin);
        _read_sample    += MQResistanceCalculation(_temp_resistance);
        delay(_READ_SAMPLE_INTERVAL);
    }

    _read_sample = _read_sample / _READ_SAMPLE_TIMES;
    return _read_sample;
}

/*****************************  MQGetPercentage **********************************
 * Input:   rs_ro_ratio - Rs divided by Ro
 *       pcurve      - pointer to the curve of the target gas
 * Output:  ppm of the target gas
 * Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm)
 *       of the line could be derived if y(rs_ro_ratio) is provided. As it is a
 *       logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic
 *       value.
 ************************************************************************************/
float const MQSensor::MQGetPercentage(const float rs_ro_ratio, const float * pcurve){
    return (pow(10, (((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
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
// ///////////////////// MQ 2 ////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

MQ2::MQ2(const uint8_t mqpin) : MQSensor(mqpin){
    // pinMode(_MQ_pin, INPUT);
};

/*
 * float const MQ2::MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id){
 *  if (gas_id == GAS_LPG) {
 *      return MQGetPercentage(rs_ro_ratio, _LPGCurve);
 *  } else if (gas_id == GAS_CO) {
 *      return MQGetPercentage(rs_ro_ratio, _COCurve);
 *  } else if (gas_id == GAS_SMOKE) {
 *      return MQGetPercentage(rs_ro_ratio, _SmokeCurve);
 *  }
 *  return 0;
 * }
 * //*/
float const MQ2::MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id){
    switch (gas_id) {
        case GAS_LPG:
            return MQGetPercentage(rs_ro_ratio, _LPGCurve);

        case GAS_CO:
            return MQGetPercentage(rs_ro_ratio, _COCurve);

        case GAS_SMOKE:
            return MQGetPercentage(rs_ro_ratio, _SmokeCurve);

        default:
            return 0;
    }
}

// Conjunto de funções de fácil legibilidade
float MQ2::readLPG(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _LPG != 0) {
        return _LPG;
    } else {
        return _LPG = MQGetGasPercentage(MQRead() / _Ro, GAS_LPG);
    }
}

float MQ2::readCO(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _CO != 0) {
        return _CO;
    } else {
        return _CO = MQGetGasPercentage(MQRead() / _Ro, GAS_CO);
    }
}

float MQ2::readSmoke(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _Smoke != 0) {
        return _Smoke;
    } else {
        return _Smoke = MQGetGasPercentage(MQRead() / _Ro, GAS_SMOKE);
    }
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////// MQ 3 ////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////

MQ3::MQ3(const uint8_t mqpin) : MQSensor(mqpin){
    // pinMode(_MQ_pin, INPUT);
};

float const MQ3::MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id){
    return MQGetPercentage(rs_ro_ratio, _C2H5OHCurve);
}

// Conjunto de funções de fácil legibilidade
float MQ3::readC2H5OH(){
    if (millis() < (_LAST_READ_TIME + _READ_SENSOR_INTERVAL) && _C2H5OH != 0) {
        return _C2H5OH;
    } else {
        // return _C2H5OH = MQGetGasPercentage(MQRead() / _Ro, GAS_C2H5OH);
        // One less function call
        return MQGetPercentage(MQRead() / _Ro, _C2H5OHCurve);
    }
}
