
//MPU9250 - 9DOF
//TSL2591 - Luminosity
//BME280 - Temperature, barometric pressure and humidity

//Testing Code

#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#include "src/TSL2591.h"
#include "src/BME280.h"
#include "src/MPU9250.h"
#include "src/TinyGPS.h"
#include "src/ArduCAM.h"
#include "src/fastlz.h"
#include "src/memorysaver.h"

const int SPI_CS = 10;

TSL2591 tsl = TSL2591();
BME280 bme = BME280(0x77);//0x76 if SDI grounded, or 0x77 if SDI is attached to logic level
MPU9250 mpu = MPU9250(0x68);// 0x68 if AD0 is grounded, 0x69 if AD0 is at logic level
TinyGPS gps;
ArduCAM myCAM( OV2640, SPI_CS );


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
    int16_t gyro[3], acc[3], magno[3];
    //float orientation[] = {0.0, 0.0, 0.0, 0.0};
    if (rhs.readGyrometerData(gyro) || rhs.readAccelometerData(acc) || rhs.readMagneoometerData(magno)){// || rhs.getQuaternion(orientation, gyro, acc, magno)) {
        lhs << "Error with MPU9250 sensor";
    } else {
        lhs << "{";
        lhs << "Gyrometer:[" << gyro[0] << "," << gyro[1] << "," << gyro[2] << "],";
        lhs << "Accelerometer:[" << acc[0] << "," << acc[1] << "," << acc[2] << "],";
        lhs << "Magnetometer:[" << magno[0] << "," << magno[1] << "," << magno[2] << "]}";
        //lhs << "Orientation:[" << orientation[0] << "," << orientation[1] << "," << orientation[2] << "," << orientation[3] << "]}";
    }
    return lhs;
}

void setup(){

    //SD Card
    Serial.begin(9600);
    while(!Serial) {}
    Wire.begin();
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
    if(mpu.start()){
        Serial.println("Couldn't connect to MPU9250 sensor");
    }

    Serial.print("Setup");

    //arducam
    uint8_t vid, pid;
    uint8_t temp;
    pinMode(SPI_CS,OUTPUT);
    SPI.begin();

    while(1){
        //Check if the ArduCAM SPI bus is OK
        myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
        temp = myCAM.read_reg(ARDUCHIP_TEST1);

        if (temp != 0x55){
          Serial.println(F("SPI interface Error!"));
          delay(1000);continue;
        }else{
          Serial.println(F("SPI interface OK."));break;
        }
    }

    //Not necessary but good for debugging
    while(1){
      //Check if the camera module type is OV2640
      myCAM.wrSensorReg8_8(0xff, 0x01);
      myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
      myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
      if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
        Serial.println(F("Can't find OV2640 module!"));
        delay(1000);continue;
      }
      else{
        Serial.println(F("OV2640 detected."));break;
      } 
    } 
    //

    //Absolutely Needed for setup
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.OV2640_set_JPEG_size(OV2640_640x480);
    delay(1000);
}

void loop() {
    String jsonData, picBytes;
    File pic;
    char ltr;
    Serial.print("Begin");

    //save the picture to a file "pic"
    myCAMSaveToString(pic);

    //move the bytes to a tring to write to json
    for (int len = 0; len < pic.size(); len++) {
      picBytes += ltr;
    }
    
    Serial.print("End");   
    jsonData << "{TSL:" << tsl << ",BME:" << bme << ",MPU: " << mpu << "PIC" << picBytes << "}";
    Serial.println(jsonData);
    Serial.println(picBytes);
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


void myCAMSaveToString(File &pic){
char str[8];
byte buf[256];
static int i = 0;
static int k = 0;
uint8_t temp = 0,temp_last=0;
uint32_t length = 0;
bool is_header = false;

//Flush the FIFO
myCAM.flush_fifo();
//Clear the capture done flag
myCAM.clear_fifo_flag();
//Start capture
myCAM.start_capture();
Serial.println("start Capture");
while(!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
Serial.println("Capture Done.");  
length = myCAM.read_fifo_length();
Serial.print("The fifo length is :");
Serial.println(length, DEC);
if (length >= MAX_FIFO_SIZE) //384K
{
  Serial.println("Over size.");
  return ;
}
if (length == 0 ) //0 kb
{
  Serial.println("Size is 0.");
  return ;
}
//Construct a file name
k = k + 1;
itoa(k, str, 10);
strcat(str, ".jpg");
//Open the new file
pic = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
if(!pic){
  Serial.println("File open faild");
  return;
}
myCAM.CS_LOW();
myCAM.set_fifo_burst();
while ( length-- )
{
  temp_last = temp;
  temp =  SPI.transfer(0x00);
  //Read JPEG data from FIFO
  if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
  {
    buf[i++] = temp;  //save the last  0XD9     
    //Write the remain bytes in the buffer
    myCAM.CS_HIGH();
    pic.write(buf, i);    
    //Close the file
    pic.close();
    Serial.println(F("Image save OK."));
    is_header = false;
    i = 0;
  }  
  if (is_header == true)
  { 
    //Write image data to buffer if not full
    if (i < 256)
    buf[i++] = temp;
    else
    {
      //Write 256 bytes image data to file
      myCAM.CS_HIGH();
      pic.write(buf, 256);
      i = 0;
      buf[i++] = temp;
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
    }        
  }
  else if ((temp == 0xD8) & (temp_last == 0xFF))
  {
    is_header = true;
    buf[i++] = temp_last;
    buf[i++] = temp;   
  } 
} 

}
