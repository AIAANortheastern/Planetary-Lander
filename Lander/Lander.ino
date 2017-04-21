
//MPU9250 - 9DOF
//TSL2591 - Luminosity
//BME280 - Temperature, barometric pressure and humidity

//Testing Code

#include <Wire.h>
#include <SD.h>

#include "src/i2c.h"
#include "src/TSL2591.h"
#include "src/BME280.h"
#include "src/MPU9250.h"
#include "src/TinyGPS.h"
#include "src/ArduCAM.h"
#include "src/fastlz.h"

TSL2591 tsl = TSL2591();
BME280 bme = BME280(0x77);//0x76 if SDI grounded, or 0x77 if SDI is attached to logic level
MPU9250 mpu;
TinyGPS gps;

template<class T> String &operator<<(String &lhs, T &rhs) {
    return lhs += rhs;
}

template<> String &operator<<(String &lhs, TSL2591 &rhs) {
    float lux;
    if (rhs.getLux(&lux)) {
        lhs << "Error with TSL2591 sensor";
    } else {
        lhs << "{Lux:" << lux << "}";
    }
    return lhs;
}

template<> String &operator<<(String &lhs, BME280 &rhs) {
    double temp, pres, humd;
    if (rhs.read_processed(&temp, &pres, &humd)) {
        lhs << "Error with BME280 sensor";
    } else {
        lhs << "{";
        lhs << "Temperature:" << temp << ",";
        lhs << "Pressure:" << pres << ",";
        lhs << "Humidity:" << humd << "}";
    }
    return lhs;
}

template<> String &operator<<(String &lhs, MPU9250 &rhs) {
    if (rhs.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {
        rhs.readAccelData(rhs.accelCount);
        rhs.getAres();
        rhs.readGyroData(rhs.gyroCount);
        rhs.getGres();
        rhs.readMagData(rhs.magCount);
        rhs.getMres();

        rhs.ax = (float) rhs.accelCount[0] * rhs.aRes;
        rhs.ay = (float) rhs.accelCount[1] * rhs.aRes;
        rhs.az = (float) rhs.accelCount[2] * rhs.aRes;

        rhs.gx = (float) rhs.gyroCount[0] * rhs.gRes;
        rhs.gy = (float) rhs.gyroCount[1] * rhs.gRes;
        rhs.gz = (float) rhs.gyroCount[2] * rhs.gRes;

        rhs.mx = (float) rhs.magCount[0] * rhs.mRes * rhs.magCalibration[0] - rhs.magbias[0];
        rhs.my = (float) rhs.magCount[1] * rhs.mRes * rhs.magCalibration[1] - rhs.magbias[1];
        rhs.mz = (float) rhs.magCount[2] * rhs.mRes * rhs.magCalibration[2] - rhs.magbias[2];

        rhs.updateTime();
        MahonyQuaternionUpdate(rhs.ax, rhs.ay, rhs.az, rhs.gx*DEG_TO_RAD, rhs.gy*DEG_TO_RAD, rhs.gz*DEG_TO_RAD, rhs.my, rhs.mx, rhs.mz, rhs.deltat);
        rhs.delt_t = millis() - rhs.count;
        float *orientation = getQ();
        rhs.count = millis();
        rhs.sumCount = 0;
        rhs.sum = 0;

        lhs << "{";
        lhs << "Gyrometer:[" << rhs.gx << "," << rhs.gy << "," << rhs.gz << "],";
        lhs << "Accelerometer:[" << 1000*rhs.ax << "," << 1000*rhs.ay << "," << 1000*rhs.az << "],";
        lhs << "Magnetometer:[" << rhs.mx << "," << rhs.my << "," << rhs.mz << "],";
        lhs << "Orientation:[" << orientation[0] << "," << orientation[1] << "," << orientation[2] << "," << orientation[3] << "]}";
    } else {
        lhs << "Error with MPU9250 sensor";
    }
    return lhs;
}

void setup(){

    //SD Card
    Serial.begin(9600);
    while(!Serial) {}
    // Pam7Q
    Serial.print("Begin");
    Serial1.begin(9600);
    while(!Serial1) {}
    Serial.println("After Serial1");
    Serial2.begin(9600);
    while(!Serial2) {}
    Serial.println("After Serial2");

    Serial3.begin(9600);
    while(!Serial3) {}
    Serial.println("After Serial3");
  
    //I2c
    Wire.begin();
    //TSL2591
    
    if(tsl.start(TSL2591_GAIN_1X, TSL2591_INTEGRATION_TIME_100MS)){
        Serial.println("Couldn't connect to TSL2591 sensor");
    }

  Serial.println("Pre BME");
    
    //BME280
    if (bme.start()) {
        Serial.println("Couldn't connect to BME280 sensor");
    }
    else {
        delay(300);
        int i = 0;
        while(bme.isReadingCalibration() && i++ < 100) delay(100);
        bme.set(BME280_16x_OVERSAMPLING, BME280_16x_OVERSAMPLING, BME280_16x_OVERSAMPLING);
    }
    //MPU9250
    if(read8(MPU9250_ADDRESS, WHO_AM_I_MPU9250) != 0x71){
        Serial.println("Couldn't connect to MPU9250 sensor");
    } else {
        mpu.calibrateMPU9250(mpu.gyroBias, mpu.accelBias);
        mpu.initMPU9250();
        if(read8(AK8963_ADDRESS, WHO_AM_I_AK8963) != 0x48){
            Serial.println("Couldn't connect to AK8963 sensor");
        } else {
            mpu.initAK8963(mpu.magCalibration);
        }
    }

    Serial.print("Setup");
}

void loop() {
    String jsonData;
    jsonData << "{TSL:" << tsl << ",BME:" << bme << ",MPU: " << mpu << "}";
    Serial.println(jsonData);
    Serial3.println(jsonData);
    //Pam7Q
    bool newdata = false;
    unsigned long start = millis();
    while (millis() - start < 5000) {
        if (feedgps()) {
            newdata = true;
        }
    }
    if (newdata) {
        Serial.print("Pam7Q: ");
        Serial3.print("Pam7Q: ");
        gpsdump(gps);
    }
    delay(50);
}



// Get and process GPS data
void gpsdump(TinyGPS &gps) {
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  Serial.print(flat, 4);
  Serial.print(". ");
  Serial.println(flon, 4);
  Serial3.print(flat, 4);
  Serial3.print(", ");
  Serial3.println(flon, 4);
}

// Feed data as it becomes available
bool feedgps() {
  while (Serial2.available()) {
    //Serial.print("Hi\n");
    //Serial.print(Serial2.read());
    if (gps.encode(Serial2.read())) {
      return true;
    }
  }
  return false;
}
