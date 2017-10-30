/*
 * Created by Daniel P. Poltronieri
 */

// TODO: MQ7 - Monóxido de Carbono

#ifndef MQSensor_h
#define MQSensor_h

#include <Arduino.h>

// Enumeration for constructor eficiency
// Located in the higher namespace possible
namespace {
enum SENSOR_TYPE : const uint8_t {
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

enum GAS_TYPE : const uint8_t {
    GAS_LPG,
    GAS_CO,
    GAS_SMOKE,
    GAS_C2H5OH,
    GAS_H2,
    GAS_MONOXIDE
};
}

/**
 * \class MQSensor
 *
 * This is a father class to all MQSensor types, it is not intended to be
 * instanciated, but can be used as a pointer array if your code needs so, also
 * can be used to instanciate and use multiple types of MQ sensor in the same
 * object.
 *
 * \author Daniel Pereira Poltronieri \date October, 2017 Contact:
 * danppoltronieri@gmail.com
 *
 *
 */
class MQSensor {
private:

public:

    // lembrar o nome dessa estrutura
    static MQSensor NewMQSensor(const uint8_t mqpin, const uint8_t mqtype);
    // This prevents instanciating a MQSensor and helps with inheritance

    /**
     * A fast read of the protected MQSensor::_MQ_pin value.
     * @return
     * A fast analogRead() of the MQSensor::_MQ_pin.
     */
    inline double check(void){
        double temp = analogRead(_MQ_pin);

        return temp;
    };

    void setRlValue(const uint8_t rlvalue);
    void setCalibrationSampleTimes(const uint8_t cst);
    void setCalibrationSampleInterval(const uint8_t csi);
    void setReadSampleInterval(const uint8_t rsi);
    void setReadSampleTimes(const uint8_t rst);
    void SetRo(const float ro_factor);
    void SetRoCleanAirFactor(const float ro_clean_air_factor);
    float const GetRoCleanAirFactor(void);
    float const GetRo(void);

    // TODO: devolver MQTension para protected
    float MQTension(const float raw_adc);

protected:
    size_t _LAST_READ_TIME  = 0,
      _READ_SENSOR_INTERVAL = 10000;
    uint8_t _MQ_pin,
      _CALIBARAION_SAMPLE_TIMES    = 5,
      _CALIBRATION_SAMPLE_INTERVAL = 50,
      _READ_SAMPLE_INTERVAL        = 50,
      _READ_SAMPLE_TIMES = 5;
    // TODO: Decidir se _RO_CLEAN_AIR_FACTOR é relevante
    float _RO_CLEAN_AIR_FACTOR = 9.83; /*!< Basically a guess, comes in the datasheet.
                                        */
    float _Ro       = 10;              /*!< Acording to the datasheet, this should be equal to _RL_VALUE in a determined concentration of the gas.
                                        * Currently unused.
                                        */
    float _RL_VALUE = 1;               /*!< The load resistance in the board, in kilo Ohms.
                                        * The sensor prototyping board comes with a 1KOhm resistor in series after the MQ Sensor.
                                        */
    MQSensor(const uint8_t mqpin);
    float const MQRead();
    float const MQGetPPM(const float sensor_tension, const float * gas_curve);
    float MQCalibration();
    float cleanAirCallibrate();
    float const inline MQResistanceCalculation(const float raw_adc);
};

/**
 * \class MQDummy
 *
 * This is a dummy sensor, to be used to generate random values for debugging purposes.
 *
 * \author Daniel Pereira Poltronieri
 * \date October, 2017
 * Contact: danppoltronieri@gmail.com
 *
 */

class MQDummy : public MQSensor {
public:

    MQDummy (const uint8_t mqpin);

    // Create a random continuous curve for the sensor reading
    inline uint16_t check(void){
        _current_value += random(-10, 10);
        if (_current_value < 0)
            _current_value = 0;
        if (_current_value > 1023)
            _current_value = 1023;
        return _current_value;
    };
protected:
    uint16_t _current_value;
};

/**
 * \class MQPotentiometer
 *
 * This uses a potentiometer as a sensor, to be used to generate values for debugging purposes.
 * May be of better use if expandanded to handle a gas curve.
 *
 * \author Daniel Pereira Poltronieri
 * \date October, 2017
 * Contact: danppoltronieri@gmail.com
 *
 */
class MQPotentiometer : public MQSensor {
public:

    MQPotentiometer(const uint8_t mqpin);

protected:
};

/**
 * \class MQ3
 *
 * The class of the MQ3 sensor, with it's relevant data.
 *
 * \author Daniel Pereira Poltronieri
 * \date October, 2017
 * Contact: danppoltronieri@gmail.com
 *
 */
class MQ3 : public MQSensor {
public:

    MQ3(const uint8_t mqpin);

    // // TODO: decidir se read(bool print) vai ser eliminado
    // float * read(bool print);
    // // Easier access function
    // float const MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id);
    float readC2H5OH();


private:
    float _C2H5OHCurve[2] = { 3.9203077, 3.01218874 };


    float _C2H5OH = 0;
};

/**
 * \class MQ7
 *
 * The class of the MQ7 sensor, with it's relevant data.
 *
 * \author Daniel Pereira Poltronieri
 * \date October, 2017
 * Contact: danppoltronieri@gmail.com
 *
 */
class MQ7 : public MQSensor {
public:


    MQ7(const uint8_t mqpin);

    float readH2();
    float readCarbonMonoxide();


private:
    float _H2Curve[3],
      _CarbonMonoxideCurve[2] = { 2.58206523, 3.04297307 };


    float _H2         = 0,
      _CarbonMonoxide = 0;
};

#endif // ifndef mq_h
