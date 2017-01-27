#ifndef LANDER_TSL2591_H
#define LANDER_TSL2591_H

#define TSL2591_ADDR        0x29
#define TSL2591_CTRL_REG    0x01
#define TSL2591_ID_REG      0x12
#define TSL2591_CHNLS_REG   0x14

//mysterious
#define TSL2591_LUX_DF     (408.0F)
#define TSL2591_LUX_COEFB  (1.64F)  // CH0 coefficient
#define TSL2591_LUX_COEFC  (0.59F)  // CH1 coefficient A
#define TSL2591_LUX_COEFD  (0.86F)  // CH2 coefficient B

typedef enum {
    TSL2591_INTEGRATION_TIME_100MS = 0x00,
    TSL2591_INTEGRATION_TIME_200MS = 0x01,
    TSL2591_INTEGRATION_TIME_300MS = 0x02,
    TSL2591_INTEGRATION_TIME_400MS = 0x03,
    TSL2591_INTEGRATION_TIME_500MS = 0x04,
    TSL2591_INTEGRATION_TIME_600MS = 0x05
} tsl2591_integration_time;

typedef enum {
    TSL2591_GAIN_1X = 0x00,
    TSL2591_GAIN_25X = 0x10,
    TSL2591_GAIN_428X = 0x20,
    TSL2591_GAIN_9876X = 0x30
} tsl2591_gain;

class TSL2591 {
    float atime, again;
    tsl2591_gain _gain;
    tsl2591_integration_time _integration;
public:
    int start(tsl2591_gain gain, tsl2591_integration_time integration);
    int set(tsl2591_gain gain, tsl2591_integration_time integration);
    int setGain(tsl2591_gain gain) { return set(gain, _integration); }
    int setIntegration(tsl2591_integration_time integration) { return set(_gain, integration); }
    int getLux(float* lux);
};

#endif //LANDER_TSL2591_H
