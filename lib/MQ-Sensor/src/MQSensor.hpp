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

class MQSensor {
private:

public:

    // lembrar o nome dessa estrutura
    static MQSensor NewMQSensor(const uint8_t mqpin, const uint8_t mqtype);
    // This prevents instanciating a MQSensor and helps with inheritance
    inline double check(void){
        double temp = analogRead(_MQ_pin);

        return temp;
    };
    // Inicia o sensor com calibragem
    // TODO: decidir se isso fica
    void begin();

    void setRlValue(const uint8_t rlvalue);
    void setCalibrationSampleTimes(const uint8_t cst);
    void setCalibrationSampleInterval(const uint8_t csi);
    void setReadSampleInterval(const uint8_t rsi);
    void setReadSampleTimes(const uint8_t rst);
    void SetRo(const float ro_factor);
    void SetRoCleanAirFactor(const float ro_clean_air_factor);
    float const GetRoCleanAirFactor(void);
    float const GetRo(void);


protected:
    size_t _LAST_READ_TIME  = 0,
      _READ_SENSOR_INTERVAL = 10000;
    uint8_t _MQ_pin,
      _CALIBARAION_SAMPLE_TIMES    = 5,
      _CALIBRATION_SAMPLE_INTERVAL = 50,
      _READ_SAMPLE_INTERVAL        = 50,
      _READ_SAMPLE_TIMES = 5;
    // TODO: Observar se é universal
    float _RO_CLEAN_AIR_FACTOR = 9.83;
    float _Ro       = 25; // o chute inicial era 10
    float _RL_VALUE = 10; // define the load resistance on the board, in kilo ohms

    MQSensor(const uint8_t mqpin);
    // MQSensor(const uint8_t mqpin, const float ro_clean_air_factor, const float ro_factor);
    float const MQRead();
    float const MQGetPercentage(const float rs_ro_ratio, const float * pcurve);
    float MQCalibration();
    float const inline MQResistanceCalculation(const float raw_adc);
};

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

class MQPotentiometer : public MQSensor {
public:

    MQPotentiometer(const uint8_t mqpin);

    /*
     * //Unnecessary
     *  inline uint16_t check(void){
     *      int16_t temp = analogRead(_MQ_pin);
     *
     *      return temp;
     *  };
     *  //*/

protected:
};

class MQ2 : public MQSensor {
public:
    // TODO: atualizar o constructor
    MQ2(const uint8_t mqpin);

    /*
     * //Foi pra superclasse
     * void SetRo(const float ro_factor);
     * void SetRoCleanAirFactor(const float ro_clean_air_factor);
     * float const GetRoCleanAirFactor(void);
     * float const GetRo(void);
     * //*/

    // DECIDIDO: decidir se read(bool print) vai ser eliminado
    // float * read(bool print);
    // Easier access function
    float const MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id);
    float readLPG();
    float readCO();
    float readSmoke();


private:
    // TODO: Averiguar essas curvas
    float _LPGCurve[3] = { 2.3, 0.21, -0.47 },
      _COCurve[3]      = { 2.3, 0.72, -0.34 },
      _SmokeCurve[3]   = { 2.3, 0.53, -0.44 };
    // two points are taken from the curve.
    // with these two points, a line is formed which is "approximately equivalent"
    // to the original curve.
    // data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)

    // _RO_CLEAN_AIR_FACTOR = 9.83,
    // _Ro = 10; // praticamente um chute

    float _LPG = 0,
      _CO      = 0,
      _Smoke   = 0;
};

class MQ3 : public MQSensor {
public:

    MQ3(const uint8_t mqpin);

    // TODO: decidir se read(bool print) vai ser eliminado
    float * read(bool print);
    // Easier access function
    float const MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id);
    float readC2H5OH();


private:
    // TODO: Averiguar essas curvas
    float _C2H5OHCurve[3] = { 0.18, 0.02, -0.16 };
    // TODO: Essa pode ser a fórmila correta
    // float _C2H5OHCurve[3] = { 0.18, 0.02, -0.00035555555555556 };
    // two points are taken from the curve.
    // with these two points, a line is formed which is "approximately equivalent"
    // to the original curve.
    // data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)


    float _C2H5OH = 0;
};

class MQ4 : public MQSensor {
public:

    MQ4(const uint8_t mqpin);

    // TODO: decidir se read(bool print) vai ser eliminado
    float * read(bool print);
    // Easier access function
    float const MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id);
    float readMethane();
    float readCH4(){
        return readMethane();
    }

    float readAlcohol();
    float readC3H8();


private:
    // TODO: Averiguar essas curvas
    float _MethaneCurve[3] = { 0.25, 0.1, -0.00021428571428571 },
    // TODO: Configurar essas curvas
      _AlcoholCurve[3] = { 2.3, 0.72, -0.34 },
      _C3H8Curve[3]    = { 2.3, 0.53, -0.44 };
    // two points are taken from the curve.
    // with these two points, a line is formed which is "approximately equivalent"
    // to the original curve.
    // data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)

    // _RO_CLEAN_AIR_FACTOR = 9.83,
    // _Ro = 10; // praticamente um chute

    float _Methane = 0,
      _Alcohol     = 0,
      _C3H8        = 0;
};

class MQ7 : public MQSensor {
public:

    MQ7(const uint8_t mqpin);

    // TODO: decidir se read(bool print) vai ser eliminado
    float * read(bool print);
    // Easier access function
    float const MQGetGasPercentage(const float rs_ro_ratio, const uint8_t gas_id);
    float readH2();
    float readCarbonMonoxide();


private:
    float _H2Curve[3] = { 1.5, 0.15, -0.014 },
    // _CarbonMonoxideCurve[3] = { 1.8, 0.2, -0.016 };
    // TODO: y = ax + b => x = y/a - b/a
      _CarbonMonoxideCurve[3] = { 1, -50, 180 };
    // two points are taken from the curve.
    // with these two points, a line is formed which is "approximately equivalent"
    // to the original curve.
    // data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)

    // _RO_CLEAN_AIR_FACTOR = 9.83,
    // _Ro = 10; // praticamente um chute

    float _H2         = 0,
      _CarbonMonoxide = 0;
};

#endif // ifndef mq_h
