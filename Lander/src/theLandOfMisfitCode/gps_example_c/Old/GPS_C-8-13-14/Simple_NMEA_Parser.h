/*

  Simple_NMEA_Parser.h

  This library provides basic NMEA parsing capabilities.  It is designed to take raw NMEA strings,
  parse the data out of them, and make the data available to a parent application.

*/

#include "simpletools.h"
#include "fdserial.h"

int gps_open(int gps_rx_pin, int gps_tx_pin, int gps_baud);
float GetLatitude();
float GetLongitude();
int GetFix();
int GetFixValid();
int GetNumTrackedSats();
float GetAltitude();
float GetHeading();
float GetVelocity();
float GetVelocityMPH();
float GetVelocityKPH();
float GetVelocityMPS();
int GetRawDate();
int GetRawTime();
float GetMagneticVariation();


