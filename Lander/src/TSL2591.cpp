#include "i2c.h"
#include "TSL2591.h"

int TSL2591::start(tsl2591_gain gain, tsl2591_integration_time integration) {
    if(read8(TSL2591_ADDR, TSL2591_ID_REG) != 0x50 ){
        return 1;
    }
    return set(gain, integration);
}

int TSL2591::set(tsl2591_gain gain, tsl2591_integration_time integration) {
    switch (gain) {
        case TSL2591_GAIN_1X:
            again = 1.0F;
            break;
        case TSL2591_GAIN_25X:
            again = 25.0F;
            break;
        case TSL2591_GAIN_428X:
            again = 428.0F;
            break;
        case TSL2591_GAIN_9876X:
            again = 9876.0F;
            break;
        default:
            return 2;
    }
    switch (integration) {
        case TSL2591_INTEGRATION_TIME_100MS :
            atime = 100.0F;
            break;
        case TSL2591_INTEGRATION_TIME_200MS :
            atime = 200.0F;
            break;
        case TSL2591_INTEGRATION_TIME_300MS :
            atime = 300.0F;
            break;
        case TSL2591_INTEGRATION_TIME_400MS :
            atime = 400.0F;
            break;
        case TSL2591_INTEGRATION_TIME_500MS :
            atime = 500.0F;
            break;
        case TSL2591_INTEGRATION_TIME_600MS :
            atime = 600.0F;
            break;
        default:
            return 2;
    }
    _gain = gain;
    _integration = integration;
    write8(TSL2591_ADDR, TSL2591_CTRL_REG, _gain | _integration);
    return 0;
}

int TSL2591::getLux(float *lux) {
    uint8_t buffer[4];
    readBuffer(TSL2591_ADDR, TSL2591_CHNLS_REG, buffer, 4);
    uint32_t ch0 = buffer[0] | ((uint32_t)buffer[1] << 8);
    uint32_t ch1 = buffer[2] | ((uint32_t)buffer[3] << 8);

    // Check for overflow conditions first
    if (ch0 == 0xFFFF | ch1 == 0xFFFF) {
        // Signal an overflow
        return 1;
    }

    float cpl = (atime * again) / TSL2591_LUX_DF;
    float lux1 = ((float) ch0 - (TSL2591_LUX_COEFB * (float) ch1)) / cpl;
    float lux2 = ((TSL2591_LUX_COEFC * (float) ch0) - (TSL2591_LUX_COEFD * (float) ch1)) / cpl;
    *lux = lux1 > lux2 ? lux1 : lux2;
    //or?
    //*lux =   ( (float)ch0 - ( 1.7F * (float)ch1 ) ) / cpl;
    return 0;
}