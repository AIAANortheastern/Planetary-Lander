//BNO055 - 9DOF
//TSL2591 - Luminosity
//BME280 - Temperature, barometric pressure and humidity

//Testing Code

#include "Wire.h"

#include "TSL2591.h"
#include "BME280.h"
#include "MPU9250.h"

TSL2591 tsl = TSL2591();
BME280 bme = BME280(0x77);//0x76 if SDI grounded, or 0x77 if SDI is attached to logic level
MPU9250 mpu = MPU9250(0x69);// 0x68 if AD0 is grounded, 0x69 if AD0 is at logic level

void setup() {
    Serial.begin(115200);
    Serial.println("Connected");
    //I2c
    if(!Wire.begin()){
        Serial.println("I2c error");
    }
    //TSL2591
    if(!tsl.start(TSL2591_GAIN_25X, TSL2591_INTEGRATION_TIME_100MS)){
      Serial.println("Couldn't connect to TSL2591 sensor");
    }
    //BME280
    if (!bme.start(BME280_NO_OVERSAMPLING, BME280_NO_OVERSAMPLING, BME280_NO_OVERSAMPLING)) {
      Serial.println("Couldn't connect to BME280 sensor");
    }
    //MPU9250
    if(!mpu.start()){
        Serial.println("Couldn't connect to MPU9250 sensor");
    }
}

void loop() {
    int16_t gyro[3], acc[3], magno[3];
    float lux, orientation[4];
    double temp, pres, humd;
    //TSL2591
    if(!tsl.getLux(&lux)){
      Serial.println("Error with TSL2591 sensor");
    } else {
        Serial.print("Lux: ");
        Serial.println(lux);
    }
    //BME280
    if (!bme.read_processed(&temp, &pres, &humd)) {
      Serial.println("Error with BME280 sensor");
    } else {
        Serial.print("Temperature: ");
        Serial.println(temp);
        Serial.print("Pressure: ");
        Serial.println(pres);
        Serial.print("Humidity: ");
        Serial.println(humd);
    }
    //MPU9250
    if(!mpu.readGyrometerData(gyro) || !mpu.readAccelometerData(acc) || !mpu.readMagneoometerData(magno) || !mpu.getQuaternion(orientation, gyro, acc, magno)){
        Serial.println("Error with MPU9250 sensor");
    } else {
        Serial.print("Gyrometer: ");
        Serial.print(gyro[0]);
        Serial.print(", ");
        Serial.print(gyro[1]);
        Serial.print(", ");
        Serial.println(gyro[2]);
        Serial.print("Accelerometer: ");
        Serial.print(acc[0]);
        Serial.print(", ");
        Serial.print(acc[1]);
        Serial.print(", ");
        Serial.println(acc[2]);
        Serial.print("Magnetometer: ");
        Serial.print(magno[0]);
        Serial.print(", ");
        Serial.print(magno[1]);
        Serial.print(", ");
        Serial.println(magno[2]);
        Serial.print("Orientation: ");
        Serial.print(orientation[0]);
        Serial.print(", ");
        Serial.print(orientation[1]);
        Serial.print(", ");
        Serial.print(orientation[2]);
        Serial.print(", ");
        Serial.println(orientation[3]);
    }
    delay(50);
}
