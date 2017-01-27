#include "mock/Wire.h"
#include "i2c.h"

int i2c_begin(){
    Wire.begin();
    return 0;
}

uint8_t read8(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, 1);
    return Wire.read();
}

uint16_t read16(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, 2);
    return (Wire.read() << 8) | Wire.read();
}

uint32_t read24(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, 3);
    return (Wire.read() << 16) | (Wire.read() << 8) | Wire.read();
}

uint32_t read32(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, 4);
    return (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8) | Wire.read();
}

uint16_t read16LE(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, 2);
    return Wire.read() | (Wire.read() << 8) ;
}

uint32_t read24LE(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, 3);
    return  Wire.read() | (Wire.read() << 8) | (Wire.read() << 16);
}

uint32_t read32LE(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, 4);
    return Wire.read() | (Wire.read() << 8) | (Wire.read() << 16) | (Wire.read() << 24);
}

void readBuffer(int addr, uint8_t reg, uint8_t * buffer, int len){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, len);
    for(int i=0; i<len; i++){
        buffer[i] = Wire.read();
    }
}

void write(int addr, uint8_t reg){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();
}

void write8(int addr, uint8_t reg, uint8_t val){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

void writeBuffer(int addr, uint8_t reg, uint8_t * values, int len){
    Wire.beginTransmission(addr);
    Wire.write(reg);
    for(int i=0; i<len; i++){
        Wire.write(values[i]);
    }
    Wire.endTransmission();
}