#include "i2c.h"
#include "BME280.h"

int BME280::start() {
    if (read8(address, BME280_CHIP_ID_REG) != 0x60) {
        return 1;
    }
    write8(address, BME280_SOFTRESET_REG, 0xB6);
    return 0;
}

int BME280::isReadingCalibration(){
    uint8_t rStatus = read8(address, BME280_STATUS_REG);
    return (rStatus & (1 << 0)) != 0;
}

void BME280::set(bme280_oversampling temperature, bme280_oversampling pressure, bme280_oversampling humidity){
    readCoefficients();
    //page 25
    write8(address, BME280_CONTROL_HUMD_REG, humidity);
    write8(address, BME280_CONFIG_REG, 0);
    write8(address, BME280_CONTROL_REG, (uint8_t) (0b11 | (uint8_t)temperature << 5 | (uint8_t)pressure << 3));
}

int BME280::readCoefficients() {
    dig_T1 = read16LE(address, 0x88);
    dig_T2 = read16LE(address, 0x8A);
    dig_T3 = read16LE(address, 0x8C);

    dig_P1 = read16LE(address, 0x8E);
    dig_P2 = read16LE(address, 0x90);
    dig_P3 = read16LE(address, 0x92);
    dig_P4 = read16LE(address, 0x94);
    dig_P5 = read16LE(address, 0x96);
    dig_P6 = read16LE(address, 0x98);
    dig_P7 = read16LE(address, 0x9A);
    dig_P8 = read16LE(address, 0x9C);
    dig_P9 = read16LE(address, 0x9E);

    dig_H1 = read8(address, 0xA1);
    dig_H2 = read16LE(address, 0xE1);
    dig_H3 = read8(address, 0xE3);
    dig_H4 = (read8(address, 0xE4) << 4) | (read8(address, 0xE5) & 0xF);
    dig_H5 = (read8(address, 0xE6) << 4) | (read8(address, 0xE5) >> 4);
    dig_H6 = read8(address, 0xE7);
    return 0;
}

int BME280::read_unprocessed(int32_t *temperature, int32_t *pressure, int32_t *humditiy) {
    uint32_t buffer[8];
    readBuffer(address, 0xF7, (uint8_t*)buffer, 8);
    *pressure = ((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]) >> 4;
    *temperature = ((buffer[3] << 16) | (buffer[4] << 8) | buffer[5]) >> 4;
    *humditiy = (buffer[6] << 8) | buffer[7];
    return 0;
}

/*
int BME280::read_processed(double *temperature, double *pressure, double *humidity) {
    int32_t temperature_adc, pressure_adc, humidity_adc, t_fine;
    //read_unprocessed(&temperature_adc, &pressure_adc, &humidity_adc);
    temperature_adc = read24(address, 0xFA);
    //sorry not mine, its the offical code in the datasheet page 23/49
    //process temperature, temperature in DegC
    int32_t var1, var2;
    temperature_adc >>= 4;

    var1 = ((((temperature_adc>>3) - ((int32_t)dig_T1 <<1))) *
            ((int32_t)dig_T2)) >> 11;

    var2 = (((((temperature_adc>>4) - ((int32_t)dig_T1)) *
              ((temperature_adc>>4) - ((int32_t)dig_T1))) >> 12) *
            ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;
    *temperature = ((t_fine * 5 + 128) >> 8)/100;
    //process pressure, pressure in Pa
    /*
    double p;
    var1 = ((double) t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double) dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double) dig_P4) * 65536.0);
    var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) dig_P1);
    if (var1 == 0.0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double) pressure_adc;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double) dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double) dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double) dig_P7)) / 16.0;
    *pressure = p;
    //process humidity, humidity in %rH
    double var_H;
    var_H = (((double) t_fine) - 76800.0);
    var_H = (humidity_adc - (((double) dig_H4) * 64.0 + ((double) dig_H5) / 16384.0 * var_H)) *
            (((double) dig_H2) / 65536.0 * (1.0 + ((double) dig_H6) / 67108864.0 * var_H *
                                                  (1.0 + ((double) dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double) dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    *humidity = var_H;
    return 0;
}*/

int BME280::read_processed(double *temperature, double *pressure, double *humidity) {
    int32_t temperature_adc, pressure_adc, humidity_adc, t_fine;
    //read_unprocessed(&temperature_adc, &pressure_adc, &humidity_adc);
    pressure_adc = read24(address, 0xF7) >> 4;
    temperature_adc = read24(address, 0xFA) >> 4;
    humidity_adc = read16(address, 0xFD);
    //sorry not mine, its the offical code in the datasheet page 23/49
    //process temperature, temperature in DegC
    double var1, var2;
    var1 = (((double) temperature_adc) / 16384.0 - ((double) dig_T1) / 1024.0) * ((double) dig_T2);
    var2 = ((((double) temperature_adc) / 131072.0 - ((double) dig_T1) / 8192.0) *
            (((double) temperature_adc) / 131072.0 - ((double) dig_T1) / 8192.0)) * ((double) dig_T3);
    t_fine = (int32_t) (var1 + var2);
    *temperature = (var1 + var2) / 5120.0;
    //process pressure, pressure in Pa
    double p;
    var1 = ((double) t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double) dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double) dig_P4) * 65536.0);
    var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) dig_P1);
    if (var1 == 0.0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double) pressure_adc;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double) dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double) dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double) dig_P7)) / 16.0;
    *pressure = p;
    //process humidity, humidity in %rH
    double var_H;
    var_H = (((double) t_fine) - 76800.0);
    var_H = (humidity_adc - (((double) dig_H4) * 64.0 + ((double) dig_H5) / 16384.0 * var_H)) *
            (((double) dig_H2) / 65536.0 * (1.0 + ((double) dig_H6) / 67108864.0 * var_H *
                                                  (1.0 + ((double) dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double) dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    *humidity = var_H;
    return 0;
}
