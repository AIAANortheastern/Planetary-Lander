//MPU9250 - 9DOF
//TSL2591 - Luminosity
//BME280 - Temperature, barometric pressure and humidity

//Testing Code

#include <Wire.h>
#include <SD.h>
#include <Servo.h>

#include "src/i2c.h"
#include "src/TSL2591.h"
#include "src/BME280.h"
#include "src/MPU9250.h"
#include "src/TinyGPS.h"
#include "src/ArduCAM.h"
#include "src/fastlz.h"
#include "src/quaternionFilters.h"

#define SerialGPS Serial4
#define SerialXBee Serial3

TSL2591 tsl = TSL2591();
BME280 bme = BME280(0x77);//0x76 if SDI grounded, or 0x77 if SDI is attached to logic level
MPU9250 mpu;
TinyGPS gps;
Servo spinServo, flipServo;

enum {
    LANDER_START,
    LANDER_LAUNCHED,
    LANDER_LANDED,
    LANDER_UPRIGHT
} LANDER_STATE;

#define GND_PRS 101250
#define GND_DIFF 700
#define CHECK_AMNT 100
int checks = 0, checks2 = 0;

boolean onGround() {
    double diff = fabs(GND_PRS - bme.pressure);
    if (diff < GND_DIFF && checks++ == CHECK_AMNT) {
        return true;
    }
    return false;
}

boolean offGround() {
    double diff = fabs(GND_PRS - bme.pressure);
    if (diff > GND_DIFF && checks2++ == CHECK_AMNT) {
        return true;
    }
    return false;
}

float uprightQuaternion[] = {0, 0, 0, 0};

boolean upright() {
    float *up = uprightQuaternion;
    const float *ort = getQ();
    float dot = up[0] * ort[0] + up[1] * ort[1] + up[2] * ort[2] + up[3] * ort[3];
    return acos(dot) < (M_PI / 6);
}

template<class T> String &operator<<(String &lhs, T &rhs) {
    return lhs += rhs;
}

template<> String &operator<<(String &lhs, TSL2591 &rhs) {
    if (rhs.getLux()) {
        lhs << "\"Error with TSL2591 sensor\"";
    } else {
        lhs << "{\"Lux\":" << rhs.lux << "}";
    }
    return lhs;
}

template<> String &operator<<(String &lhs, BME280 &rhs) {
    if (rhs.read_processed()) {
        lhs << "\"Error with BME280 sensor\"";
    } else {
        lhs << "{";
        lhs << "\"Temperature\":" << rhs.temperature << ",";
        lhs << "\"Pressure\":" << rhs.pressure << ",";
        lhs << "\"Humidity\":" << rhs.humidity << "}";
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
        //MadgwickQuaternionUpdate(rhs.ax, rhs.ay, rhs.az, rhs.gx*DEG_TO_RAD, rhs.gy*DEG_TO_RAD, rhs.gz*DEG_TO_RAD, rhs.my, rhs.mx, rhs.mz, rhs.deltat);
        MahonyQuaternionUpdate(rhs.ax, rhs.ay, rhs.az, rhs.gx * DEG_TO_RAD, rhs.gy * DEG_TO_RAD, rhs.gz * DEG_TO_RAD,
                               rhs.my, rhs.mx, rhs.mz, rhs.deltat);
        rhs.delt_t = millis() - rhs.count;
        const float *orientation = getQ();
        rhs.count = millis();
        rhs.sumCount = 0;
        rhs.sum = 0;

        lhs << "{";
        lhs << "\"Gyrometer\":[" << rhs.gx << "," << rhs.gy << "," << rhs.gz << "],";
        lhs << "\"Accelerometer\":[" << rhs.ax << "," << rhs.ay << "," << rhs.az << "],";
        lhs << "\"Magnetometer\":[" << rhs.mx << "," << rhs.my << "," << rhs.mz << "],";
        lhs << "\"Orientation\":[" << orientation[0] << "," << orientation[1] << "," << orientation[2] << ","
            << orientation[3] << "]}";
    } else {
        lhs << "\"Error with MPU9250 sensor\"";
    }
    return lhs;
}

template<> String &operator<<(String &lhs, TinyGPS &rhs) {
    float flat, flon;
    unsigned long age;
    while (SerialGPS.available()) {
        if (rhs.encode(SerialGPS.read())) {
            rhs.f_get_position(&flat, &flon, &age);
            lhs << "[" << flat << "," << flon << "]";
            return lhs;
        }
    }
    lhs << "\"No New Data\"";
    return lhs;
}

float battery() {
    return analogRead(23) * 0.01730355;
}

void setup() {
    File dataFile = SD.open("run.txt", FILE_WRITE);
    Serial.begin(9600);
    while (!Serial) {}
    Serial.print("Begin");
    // Pam7Q
    SerialGPS.begin(9600);
    while (!SerialGPS) {}
    SerialXBee.begin(9600);
    while (!SerialXBee) {}
    //I2c
    Wire.begin();
    //TSL2591
    if (tsl.start(TSL2591_GAIN_1X, TSL2591_INTEGRATION_TIME_100MS)) {
        Serial.println("\"Couldn't connect to TSL2591 sensor\"");
        SerialXBee.println("\"Couldn't connect to TSL2591 sensor\"");
        dataFile.println("\"Couldn't connect to TSL2591 sensor\"");
    }
    //BME280
    if (bme.start()) {
        Serial.println("\"Couldn't connect to BME280 sensor\"");
        SerialXBee.println("\"Couldn't connect to BME280 sensor\"");
        dataFile.println("\"Couldn't connect to BME280 sensor\"");
    } else {
        delay(300);
        int i = 0;
        while (bme.isReadingCalibration() && i++ < 100) delay(100);
        bme.set(BME280_16x_OVERSAMPLING, BME280_16x_OVERSAMPLING, BME280_16x_OVERSAMPLING);
    }
    //MPU9250
    if (read8(MPU9250_ADDRESS, WHO_AM_I_MPU9250) != 0x71) {
        Serial.println("\"Couldn't connect to MPU9250 sensor\"");
        SerialXBee.println("\"Couldn't connect to MPU9250 sensor\"");
        dataFile.println("\"Couldn't connect to MPU9250 sensor\"");
    } else {
        mpu.calibrateMPU9250(mpu.gyroBias, mpu.accelBias);
        mpu.initMPU9250();
        if (read8(AK8963_ADDRESS, WHO_AM_I_AK8963) != 0x48) {
            Serial.println("\"Couldn't connect to AK8963 sensor\"");
            SerialXBee.println("\"Couldn't connect to AK8963 sensor\"");
            dataFile.println("\"Couldn't connect to AK8963 sensor\"");
        } else {
            mpu.initAK8963(mpu.magCalibration);
        }
    }
    //Battery
    pinMode(23, INPUT);
    analogReadResolution(10);
    //Servos
    spinServo.attach(2);
    flipServo.attach(4);
    dataFile.close();
}

void loop() {
    String jsonData;
    int mtime;
    float bat;
    if (LANDER_STATE < LANDER_UPRIGHT) {
        jsonData << "[";
        for (int i = 0; i < 20; i++) {
            mtime = millis();
            bat = battery();
            jsonData << "{\"Time\":" << mtime << ",\"TSL\":" << tsl << ",\"BME\":" << bme << ",\"MPU\": " << mpu
                     << ",\"Battery\":" << bat << ",\"GPS\":" << gps << "},";
            if (LANDER_STATE == LANDER_START && offGround()){
                LANDER_STATE = LANDER_LAUNCHED;
                break;
            }
            else if (LANDER_STATE == LANDER_LAUNCHED && onGround()){
                LANDER_STATE = LANDER_LANDED;
                break;
            }
            else if (LANDER_STATE == LANDER_LANDED && upright()) {
                LANDER_STATE = LANDER_UPRIGHT;
                break;
            }
            delay(50);
        }
        jsonData << "]";
    } else {
        mtime = millis();
        bat = battery();
        jsonData << "{\"Time\":" << mtime << ",\"TSL\":" << tsl << ",\"BME\":" << bme << ",\"MPU\": " << mpu
                 << ",\"Battery\":" << bat << ",\"GPS\":" << gps << "}";
        delay(1000);
    }
    File dataFile = SD.open("run.txt", FILE_WRITE);
    Serial.println(jsonData);
    SerialXBee.println(jsonData);
    dataFile.println(jsonData);
    dataFile.close();
}
