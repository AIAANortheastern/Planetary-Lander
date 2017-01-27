#include "i2c.h"
#include "BME280.h"
#include "TSL2591.h"
#include "MPU9250.h"

BME280 bme = BME280(0x76);
TSL2591 tsl2591 = TSL2591();
MPU9250 mpu9250 = MPU9250(0x68);

void begin();
void loop();

int main(){
    begin();
    while (1){
        loop();
    }
    return 0;
}

//TODO error checking
void begin(){
    int status;
    status = i2c_begin();
    status = bme.start(BME280_1x_OVERSAMPLING, BME280_1x_OVERSAMPLING, BME280_1x_OVERSAMPLING);
    status = tsl2591.start(TSL2591_GAIN_25X, TSL2591_INTEGRATION_TIME_500MS);
    status = mpu9250.start();
}

void loop(){
    double temperature, pressure, humditiy;
    float lux, orientation[4];
    bme.read_processed(&temperature, &pressure, &humditiy);
    tsl2591.getLux(&lux);
    mpu9250.getQuaternion(orientation);
}