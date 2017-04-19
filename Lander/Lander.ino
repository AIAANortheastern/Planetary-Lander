
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

TSL2591 tsl = TSL2591();
BME280 bme = BME280(0x77);//0x76 if SDI grounded, or 0x77 if SDI is attached to logic level
MPU9250 mpu = MPU9250(0x68);// 0x68 if AD0 is grounded, 0x69 if AD0 is at logic level
TinyGPS gps;

// Stuff for camera to function appropriately
#define SD_CS 9
const int SPI_CS = 10; // used to be 7, redefined it to be 15 for current wiring.
#if defined (OV2640_MINI_2MP)
  ArduCAM myCAM(OV2640, SPI_CS);
#else
  ArduCAM myCAM(OV5642, SPI_CS);
#endif





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

    // ArduCAM
    uint8_t vid, pid;
    uint8_t temp;
    pinMode(SPI_CS,OUTPUT);
    SPI.begin();
    int errorCount = 0;
    while(1) {
      // Check if the ArduCAM SPI bus is OK
      myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
      temp = myCAM.read_reg(ARDUCHIP_TEST1);

      if (temp != 0x55) {
        Serial.println(F("SPI interface Error!"));
        errorCount++;
        delay(1000);
      } else {
        Serial.println(F("SPI interface OK."));
        break;
      }
      if (errorCount > 10) {
        break;
      }
    }

    #if defined (OV2640_MINI_2MP)
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
    #else
      while(1){
        //Check if the camera module type is OV5642
        myCAM.wrSensorReg16_8(0xff, 0x01);
        myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
        myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
        if((vid != 0x56) || (pid != 0x42)){
          Serial.println(F("Can't find OV5642 module!"));
          delay(1000);continue;
        }
        else{
          Serial.println(F("OV5642 detected."));break;
        } 
      }
    #endif
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    #if defined (OV2640_MINI_2MP)
      myCAM.OV2640_set_JPEG_size(OV2640_640x480);
    #else
      myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
      myCAM.OV5642_set_JPEG_size(OV5642_320x240);
    #endif

    Serial.println("Setup Completed");
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

void myCAMSaveToSDFile(){
  char str[8];
  byte buf[256];
  static int i = 0;
  static int k = 0;
  uint8_t byte = 0,byte_last=0;
  uint32_t length = 0;
  bool is_header = false;
  bool print = false;
  File outFile;
  //Flush the FIFO
  myCAM.flush_fifo();
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Start capture
  myCAM.start_capture();
  Serial.println(F("start Capture"));
  while(!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
  Serial.println(F("Capture Done."));  
  length = myCAM.read_fifo_length();
  Serial.print(F("The fifo length is :"));
  Serial.println(length, DEC);
  if (length >= MAX_FIFO_SIZE) //384K
  {
    Serial.println(F("Over size."));
    return ;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
    return ;
  }
  //Construct a file name
  k = k + 1;
  itoa(k, str, 10);
  strcat(str, ".jpg");
  //Open the new file
///////////////////////////////////////////////////////////////
  //outFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
  // if(!outFile){
  //   Serial.println(F("File open faild"));
  //   return;
  // }
///////////////////////////////////////////////////////////////

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  int num = 1;
  while ( length-- )
  {
    byte_last = byte;
    byte = SPI.transfer(0x00);

    if ( (byte == 0xD8 && byte_last == 0xFF) || print == true )
    {
      print = true;
      if (byte_last<0x10) {Serial.print("0");}
      Serial.print(byte_last, HEX);
      Serial.print(" ");
      num++;

      if (num % 40 == 0)
      {
        //Serial.print(' ');
        //Serial.print(num);
        Serial.print("\n");
      }

      if (byte == 0xD9 && byte_last == 0xFF)
      {
        print = false;
        Serial.print(byte, HEX);
        Serial.print("\n");
      }
    }
  }
}
    //Read JPEG data from FIFO
//     if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
//     {
//       //buf[i++] = temp;  //save the last  0XD9     
//       //Write the remain bytes in the buffer
//       myCAM.CS_HIGH();

// //////////////////////////////////////////////////////////////////
//       //outFile.write(buf, i);  
//       //Close the file
//       //outFile.close();
 
//       //Serial.println("Here comes photo data.");
//       //for (int num=0; num<i; num++)
//       //{
//         Serial.print(temp, HEX);
//         num++;
//         if (num % 80 == 0)
//         {
//           Serial.print("\n");
//           num = 1;
//         }
//       //}
//       //Serial.print("\nEnd of photo data.\n");
// //////////////////////////////////////////////////////////////////

//       Serial.println(F("Image save OK."));
//       is_header = false;
//       //i = 0;
//     }  
//     if (is_header == true)
//     { 
//       //Write image data to buffer if not full
//       //if (i < 256)
//       //buf[i++] = temp;
//       //else
//       //{
//         //Write 256 bytes image data to file
//         myCAM.CS_HIGH();
//         ///////////////////////////////////////////////////
//         //outFile.write(buf, 256);
//         //Serial.print("\nStart of header data.\n");
//         //Serial.write(buf, i);
//         //Serial.print("\nEnd of header data.\n");
//         //for (int num=0; num<i; num++)
//         //{
//           Serial.print(temp, HEX);
//           num++;
//           if (num % 80 == 0)
//           {
//            Serial.print("\n");
//            num = 1;
//           }
//         //}
// ///////////////////////////////////////////////////////
//         //i = 0;
//         //buf[i++] = temp;
//         myCAM.CS_LOW();
//         myCAM.set_fifo_burst();
//       }        
//     //}
//     else if ((temp == 0xD8) & (temp_last == 0xFF))
//     {
//       is_header = true;
//       //buf[i++] = temp_last;
//       //buf[i++] = temp;
//       Serial.print(temp_last, HEX);
//       num++;
//       Serial.print(temp, HEX);
//       num++;
//       if (num % 80 == 0 )
//       {
//         Serial.print("\n");
//         num = 1;
//       }   
//     } 
//  } 
//}






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
