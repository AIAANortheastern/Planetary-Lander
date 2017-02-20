#ifndef LANDER_MPU9250_H
#define LANDER_MPU9250_H

#include <stdint.h>

#define MPU9250_ACCEL_REG 0x3B
#define MPU9250_GYRO_REG 0x43
#define MPU9250_ID_REG 0x75
#define MPU9250_ID 0x71

#define AK8963_DATA_REG 0x03
#define AK8963_ID_REG 0x00
#define AK8963_ID 0x48
#define AK8963_CTRL1_REG 0x0A


typedef enum {
    MPU9250_GYRO_RANGE_250DPS,
    MPU9250_GYRO_RANGE_500DPS,
    MPU9250_GYRO_RANGE_1000DPS,
    MPU9250_GYRO_RANGE_2000DPS,
} mpu9250_gyro_range;

typedef enum {
    MPU9250_ACCEL_RANGE_2G,
    MPU9250_ACCEL_RANGE_4G,
    MPU9250_ACCEL_RANGE_8G,
    MPU9250_ACCEL_RANGE_16G,
} mpu9250_accel_range;

class AK8963 {
    uint8_t address = 0X0C;
public:
    int start();
    int read(int16_t* output);
};

class MPU9250 {
    uint8_t address;
    AK8963 mageno;
    float q[4] = {1.0f, 0.0f, 0.0f, 0.0f}, beta = 0.9069, dt = 0;
    void Quaternion(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
public:
    MPU9250(uint8_t addr): address(addr), mageno() {}; // 0x68 if AD0 is grounded, 0x69 if AD0 is at logic level
    int start();
    int readAccelometerData(int16_t* output); // m/s^2
    int readGyrometerData(int16_t* output); // rps
    int readMagneoometerData(int16_t* output); // uT
    int getQuaternion(float *output);
    int getQuaternion(float *output, int16_t gyro[], int16_t acc[], int16_t magno[]);
};

#endif //LANDER_MPU9250_H
