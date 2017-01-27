#ifndef LANDER_WIRE_H
#define LANDER_WIRE_H

#include <stdint.h>

enum {
    WIRE_OFF,
    WIRE_ON,
    TRANSMISSION_BEGUN,
    TRANSMISSION_ENDED,
    REQUEST_ENDED
};

class Wire {
    int state;
public:
    void begin();

    void beginTransmission(int i);

    void endTransmission();

    void requestFrom(int i, int i1);

    void write(uint8_t i);

    uint8_t read();
} Wire;

#endif //LANDER_WIRE_H
