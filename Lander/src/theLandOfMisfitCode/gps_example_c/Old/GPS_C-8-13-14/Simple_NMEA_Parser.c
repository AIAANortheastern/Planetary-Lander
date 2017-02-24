/*

  Simple_NMEA_Parser.c

  This library provides basic NMEA parsing capabilities.  It is designed to take raw NMEA strings,
  parse the data out of them, and make the data available to a parent application through accessor
  functions.

*/

#ifndef __SIMPLE_NMEA_PARSER__
#define __SIMPLE_NMEA_PARSER__

#if defined(__cplusplus)
#extern "C" {
#endif


#include "simpletools.h"
#include "fdserial.h"
#include "string.h"
#include "math.h"

#define INBUFF_SIZE    128  //needs to be big enough to hold an entire NMEA sentence and a few estra bytes
#define TRUE           1
#define FALSE          0

#define KNOTS          0
#define MPH            1
#define KPH            2
#define MPS            3


//Type definitions
typedef unsigned char byte;

typedef struct nmea_data_s
{
  int fix;            //fix quality, 0=invalid, 1=GPS, 2=DGPS, etc...
  int fix_valid;      //boolean indicating a valid GPS fix
  float lat_dds;      //current latitude in decimal degress
  float lon_dds;      //current longitude in decimal degrees
  int sats_tracked;   //current number of satellites tracked by the GPS
  float altitude;     //current altitude, in meters, as float
  float heading;      //current direction of travel, in degrees, as float
  float velocity;     //current speed if travel, in knots, as float
  float date;         //current date, raw format with tenths of second, as float
  int time;           //current UTC time, raw format, as integer
  float mag_var;      //current magnetic variation, as float

} nmea_data;


//Function Prototypes
void gps_run(void *par);
void ParseGGA();
void ParseRMC();
void PrepBuff();

//Global variable declarations
int _gps_rx_pin, _gps_tx_pin, _gps_baud;
nmea_data gps_data;
fdserial *gps_ser;

int  cog = -1;
int  stopping;
int  stack[100];

byte inBuff[INBUFF_SIZE];
byte *ptrBuff;

int gps_open(int gpsSin, int gpsSout, int gps_baud)   // Open reader, start reading
{

  stopping = 0;
  cog = cogstart(&gps_run, NULL, stack, sizeof(stack));

  if(cog < 0)
  {
    //a valid cog was NOT grabbed, clear the GPS data structure and pin info
    memset(&gps_data, 0, sizeof(nmea_data));
    memset(&_gps_rx_pin, 0, (sizeof(int)*3));         
  }
  else
  {
    //the GPS parser cog was started
    _gps_rx_pin = gpsSin;
    _gps_tx_pin = gpsSout;
    _gps_baud = gps_baud;
  }

  return(cog < 0 ? FALSE:TRUE);
}


int gps_close()
{
  if(cog >= 0)
  {
    memset(&gps_data, 0, sizeof(nmea_data));          //clear the GPS data structure
    stopping = 1;

    while(stopping)
      pause(1);

    cogstop(cog);
    cog = -1;
  }
}

int gps_changeBaud(int newBaudRate)
{
  gps_close();
  pause(50);
  return(gps_open(_gps_rx_pin, _gps_tx_pin, newBaudRate));
}

float gps_latitude()
{
  return(gps_data.lat_dds);
}

float gps_longitude()
{
  return(gps_data.lon_dds);
}

int gps_fix()
{
  return(gps_data.fix);
}

int gps_fixValid()
{
  return(gps_data.fix_valid);
}

int gps_numTrackedSats()
{
  return(gps_data.sats_tracked);
}

float gps_altitude()
{
  return(gps_data.altitude);
}

float gps_heading()
{
  return(gps_data.heading);
}

float gps_velocity(int unit_type)
{
/*
  Returns the velocity measurement from the GPS, in the desired predefined unit type.
*/
  float vel = gps_data.velocity;

  switch(unit_type)
  {
    case KNOTS:
      break;
    case MPH:
      //Conversion, knots to miles per hour (MPH).
      //1 Knot = 1.15078 MPH
      vel *= 1.15078;
      break;
    case KPH:
      //Conversion, knots to kilometers per hour (KPH).
      //1 Knot = 1.852 KPH
      vel *= 1.852;
      break;
    case MPS:
      //Conversion, knots to meters per second (mps).
      //1 Knot = .5144444 m/s
      vel *= 0.514444444444444;
      break;
    default:
      //invalid type specifier
      vel = -1;
  }
  return(vel);
}

int gps_rawDate()
{
  return(gps_data.date);
}

int gps_rawTime()
{
  return(gps_data.time);
}

float gps_magneticVariation()
{
  return(gps_data.mag_var);
}



void gps_run(void *par)
{
  byte tempBuff[16];
  byte ch;
  int idx;

  gps_ser = fdserial_open(_gps_rx_pin, _gps_tx_pin, 0, _gps_baud);
  for(;;)
  {
    if(stopping)
    {

      fdserial_close(gps_ser);
      stopping = 0;
    }
    ch = fdserial_rxChar(gps_ser);
    
    //search for the start of an NMEA sentence
    if(ch != '$')
      continue;


    //read in characters from the GPS
    idx = 0;
    do
    {
      ch = fdserial_rxChar(gps_ser);
      inBuff[idx++] = ch;      
    }while(ch != 13);
    inBuff[idx] = 0;      //null terminate

    //got the full sentence, do a little prep work to get ready for parsing.
    //modifies inBuff!
    PrepBuff();

    if(strncmp(inBuff, "GPRMC", 5) == 0)
      ParseRMC();
    if(strncmp(inBuff, "GPGGA", 5) == 0)
      ParseGGA();
  }
}

void ParseRMC()
{
  int i;
  float f_temp;

  ptrBuff = strtok(inBuff,",");
  i=0;  
  while(ptrBuff && i<12)
  {
    if(i==1)  //time in RMC sentence, raw format, as float
      gps_data.time = atof(ptrBuff);

    if(i==2)  //Fix status
      gps_data.fix_valid = strcmp(ptrBuff, "A") ? FALSE:TRUE;

    if(i==3)  //latitude field in RMC sentence
    {
      int sign;
      int degs;
      f_temp = (float)atof(ptrBuff);
      ptrBuff = strtok(NULL,",");
      i++;

      //sign = (abs(strcmp(ptrBuff,"S"))<<1)-1;
      sign = strcmp(ptrBuff,"N") ? -1:1;  //create a sign multiplier from N/S
      degs = (int)f_temp/100;   //grab the whole number of degrees
      f_temp -= (degs*100);     //remove the degrees from the calculation
      gps_data.lat_dds = sign*((f_temp/60)+degs);  //calculate decimal degrees from remaining minutes, then add back degrees and apply the sign
    }

    if(i==5)  //longitude field in RMC sentence
    {
      int sign;
      int degs;
      f_temp = (float)atof(ptrBuff);
      ptrBuff = strtok(NULL,",");
      i++;

      //these next few lines convert
      //  degs and mins to decimal degree seconds

      //sign = (abs(strcmp(ptrBuff,"W"))<<1)-1;
      sign = strcmp(ptrBuff,"E") ? -1:1;  //create a sign multiplier from E/W
      degs = (int)f_temp/100;   //grab the whole number of degrees
      f_temp -= (degs*100);     //remove the degrees from the calculation
      gps_data.lon_dds = sign*((f_temp/60)+degs);  //calculate decimal degrees from remaining minutes, then add back degrees and apply the sign
    }

    if(i==7)  //speed field in RMC sentence, in knots
      gps_data.velocity = (float)atof(ptrBuff);
    
    if(i==8)  //heading angle in RMC sentence, in degrees
      gps_data.heading = (float)atof(ptrBuff);
    
    if(i==9)  //date in RMC sentence, raw format, as integer
      gps_data.date = atoi(ptrBuff);

    if(i==10) //magnetic variation in RMC sentence, in degrees
      gps_data.mag_var = (float)atof(ptrBuff);
    
    ptrBuff = strtok(NULL,",");
    i++;
  }


}

void ParseGGA()
{
  int i;  //i will contain the field number of the comma separated string.

  ptrBuff = strtok(inBuff,",");
  i=0;  
  while(ptrBuff && i<16)
  {
    if(i==6)  //fix quality in GGA sentence
      gps_data.fix = atoi(ptrBuff);
    
    if(i==7)  //number of satellites tracked in GGA sentence
      gps_data.sats_tracked = atoi(ptrBuff);

    if(i==9) //altitude of receiver, in meters, as a float
      gps_data.altitude = (float)atof(ptrBuff);
    
    ptrBuff = strtok(NULL,",");
    i++;
  }
}

void PrepBuff()
{
  //this is a private helper function to add ascii 0's to empty strings in the rxed NMEA sentence
  //this is needed by strtok so as not to skip over empty strings.
  int ch, prevch=0, len;
  len = strlen(inBuff);

  for(int i=0; i<len; i++)
  {
    ch = inBuff[i];
    if(ch == ',' && prevch == ',')  //have we seen two ',' in a row?
    {
      memmove(inBuff+i+1, inBuff+i, len-i); //make room for a byte in the buffer
      prevch = inBuff[i] = '0';     //insert the '0', update the last byte seen
      len++;                        //we grew the buffer's length, so account for it
    }
    else
    {
      prevch = ch;  //remember this byte as the last byte seen
    }
  }
}


#if defined(__cplusplus)
}
#endif
//  end __cplusplus

#endif
//  end __SIMPLE_NMEA_PARSER__ redefinition guard
