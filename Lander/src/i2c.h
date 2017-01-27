#ifndef LANDER_I2C_H
#define LANDER_I2C_H

#include <stdint.h>

int i2c_begin();
//Big Endian
uint8_t read8(int addr, uint8_t reg);
uint16_t read16(int addr, uint8_t reg);
uint32_t read24(int addr, uint8_t reg);
uint32_t read32(int addr, uint8_t reg);
//Little Endian
uint16_t read16LE(int addr, uint8_t reg);
uint32_t read24LE(int addr, uint8_t reg);
uint32_t read32LE(int addr, uint8_t reg);

void write(int addr, uint8_t reg);
void write8(int addr, uint8_t reg, uint8_t val);

void readBuffer(int addr, uint8_t reg, uint8_t * buffer, int len);
void writeBuffer(int addr, uint8_t reg, uint8_t * values, int len);

#endif //LANDER_I2C_H
