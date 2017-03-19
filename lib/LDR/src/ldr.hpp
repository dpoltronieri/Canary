/*
 * Created by Daniel P. Poltronieri, March, 4, 2017.
 */

#ifndef LDR_h
#define LDR_h

#include <Arduino.h>

class ldr {
public:

    ldr(const uint8_t ldrpin)
        : _ldr_pin {
        ldrpin
    } { }

    // This prevents instanciating a LineController and helps with inheritance
    inline uint16_t check(void){
        int16_t temp = analogRead(_ldr_pin);

        return temp;
    };

protected:

    uint8_t _ldr_pin;
};

#endif // ifndef LDR_h
