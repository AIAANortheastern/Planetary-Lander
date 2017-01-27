#ifndef LANDER_BME280_H
#define LANDER_BME280_H

#define BME280_CHIP_ID_REG 0xD0
#define BME280_CONTRL_HUMD_REG 0xF2
#define BME280_CONTRL_REG 0xF4

typedef enum {
    BME280_NO_OVERSAMPLING = 0x00,
    BME280_1x_OVERSAMPLING = 0x01,
    BME280_2x_OVERSAMPLING = 0x02,
    BME280_4x_OVERSAMPLING = 0x03,
    BME280_8x_OVERSAMPLING = 0x04,
    BME280_16x_OVERSAMPLING = 0x05,
} bme280_oversampling;

class BME280 {
    uint8_t address;
    //calibration numbers built into the chip during manufacturing
    int readCoefficients();
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
public:
    BME280(uint8_t addr): address(addr){}; //0x76 if SDI grounded, or 0x77 if SDI is attached to logic level
    int start(bme280_oversampling temperature, bme280_oversampling pressure, bme280_oversampling humidity);
    int read_unprocessed(int32_t* temperature, int32_t* pressure, int32_t* humditiy);
    int read_processed(double *temperature, double *pressure, double *humidity);
};
#endif //LANDER_BME280_H
