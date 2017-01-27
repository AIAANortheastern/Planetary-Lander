//BNO055 - 9DOF
//TSL2591 - Luminosity
//BME280 - Temperature, barometric pressure and humidity

//offical
#include <Wire.h>
#include <utility/imumaths.h>
#include <Servo.h>
//adafruit
#include <Adafruit_BNO055.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2591.h>
//other
#include <XBee.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_BME280 bme = Adafruit_BME280();
Adafruit_TSL2591 tsl = Adafruit_TSL2591();

Servo hinge;
#define HINGE_PIN 9

XBee xbee = XBee();
//address of ground station XBee
#define ADDR 0x001 /* TODO find the real one*/

struct sensor_data {
  //BME280 data
  float temp, pressure, humidity;
  //TSL2591 data
  uint32_t lux;
  //BNO055 data
  imu::Vector<3> acceleration, magnetometer, gyroscope;
  imu::Quaternion quaternion;
};

void collect(struct sensor_data& data){
  //BME280
  data.temp = bme.readTemperature(); // Celisus
  data.pressure = bme.readPressure(); // Pascals
  data.humidity = bme.readHumidity(); // Percent Humidity
  //TSL2591
  uint32_t lum = tsl.getFullLuminosity(); // raw sensor data
  uint16_t ir = lum >> 16, full = lum & 0xFFFF;
  data.lux = tsl.calculateLux(full, ir); // data in SI units
  //BNO055
  data.acceleration = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER); // m/s^2
  data.magnetometer = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER); // uT
  data.gyroscope = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE); // rps
  data.quaternion = bno.getQuat(); // degrees
}

String VtS(imu::Vector<3>& v){
  return "{\"x\":" + String(v.x()) +
  ",\"y\":" + String(v.y()) +
  ",\"z\":" + String(v.z()) + "}";
}

String QtS(imu::Quaternion& v){
  return "{\"w\":" + String(v.w()) +
  ",\"x\":" + String(v.x()) +
  ",\"y\":" + String(v.y()) +
  ",\"z\":" + String(v.z()) + "}";
}

String packetize(struct sensor_data& data){
  String packet = "{";
  //BME280
  packet += "\"temperature\":" + String(data.temp);
  packet += ",\"pressure\":" + String(data.pressure);
  packet += ",\"humidity\":" + String(data.humidity);
  //TSL2591
  packet += ",\"lux\":" + String(data.lux);
  //BNO055
  packet += ",\"acceleration\":" + VtS(data.acceleration);
  packet += ",\"magnetometer\":" + VtS(data.magnetometer);
  packet += ",\"gyroscope\":" + VtS(data.gyroscope);
  packet += ",\"quaternion\":" + QtS(data.quaternion);

  return packet + "}";
}

void setup() {
  //XBEE
  Serial.begin(115200);
  xbee.setSerial(Serial);
  //BME280
  if (!bme.begin()) {
    Serial.println("Couldn't connect to BME280 sensor");
  }
  //TSL2591
  if(!tsl.begin()){
    Serial.println("Couldn't connect to TSL2591 sensor");
  }
  tsl.setGain(TSL2591_GAIN_MED);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
  //BNO055
  if(!bno.begin()){
    Serial.println("Couldn't connect to BNO055 sensor");
  }
  bno.setExtCrystalUse(true);
  //Servo
  hinge.attach(HINGE_PIN);
}

void loop() {
  struct sensor_data data;
  collect(data);
  String packet = packetize(data);
  Tx16Request payload = Tx16Request(ADDR, packet.c_str(), packet.length());
  xbee.send(payload);
  hinge.write(90);
  delay(500);
}
