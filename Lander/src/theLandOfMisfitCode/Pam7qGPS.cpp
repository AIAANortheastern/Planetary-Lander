#include "Pam7qGPS.h"

volatile int ptr = 0;
volatile bool flag = true;
volatile char redbuffer[MAXLENGTH];
volatile char blubuffer[MAXLENGTH];

char gpsfields[MAXNUMFIELDS][MAXFIELDLENGTH];

Pam7qGPS::Pam7qGPS(){

}

void Pam7qGPS::read(char nextChar){

  // Start of a GPS message
  if (nextChar == '$') {
    if (flag) {
      redbuffer[ptr] = '\0';
    }
    else {
      blubuffer[ptr] = '\0';
    }
    ptr = 0;
  }

  // End of a GPS message
  if (nextChar == '\n') {

    if (flag) {
      flag = false;
      redbuffer[ptr] = '\0';
      if (CheckSum((char*) redbuffer )) {
        //Serial.println((char*) redbuffer );
        ParseString((char*) redbuffer );
      }
    }
    else
    {
      flag = true;
      blubuffer[ptr] = '\0';
      if (CheckSum((char*) blubuffer )) {
        //Serial.println((char*) blubuffer );
        ParseString((char*) blubuffer );
      }
    }   
    CheckGPGGA();
    CheckGPRMC();
    ptr = 0; 
  }

  // Add a new character
  if (flag) {
    redbuffer[ptr] = nextChar;
  }
  else {
    blubuffer[ptr] = nextChar;
  }

  ptr++;
  if (ptr >= MAXLENGTH) {
    ptr = MAXLENGTH-1;
  }
}

bool Pam7qGPS::CheckSum(char* msg) {

  // Check the checksum
  //$GPGGA,.........................0000*6A



  // Length of the GPS message
  int len = strlen(msg);

  // Does it contain the checksum, to check
  if (msg[len-4] == '*') {

	// Read the checksum from the message
	int cksum = 16 * Hex2Dec(msg[len-3]) + Hex2Dec(msg[len-2]);

	// Loop over message characters
	for (int i=1; i < len-4; i++) {
      	  cksum ^= msg[i];
    	}

	// The final result should be zero
	if (cksum == 0){
	  return true;
	}
  }

  return false;
}


void Pam7qGPS::CheckGPGGA() {

// $GPGGA,130048.000,5116.2866,N,00008.6929,E,1,10,1.1,75.5,M,47.0,M,,0000*62



  // Do we have a GPGGA message?
  if (strstr(gpsfields[0], "$GPGGA")) {

    time = atof(gpsfields[1]);      // 130048.000
    latitude = atof(gpsfields[2]);  // 5116.2866
    latNS = gpsfields[3][0];        // N
    longitude = atof(gpsfields[4]); // 00008.6929
    lonEW = gpsfields[5][0];        // E
                                    // Fix quality (1=GPS)(2=DGPS)
                                    // Number of satellites being tracked
                                    // Horizontal dilution of position
                                    // Altitude above mean sea level, Meters 
                                    // Height of geoid (mean sea level)
                                    // Time in seconds since last DGPS update
                                    // DGPS station ID number
                                    // the checksum data, always begins with *

    latitude = DegreeToDecimal(latitude, latNS);
    longitude = DegreeToDecimal(longitude, lonEW);
  }
}


void Pam7qGPS::CheckGPRMC() {

//$GPRMC,111837.000,A,5117.2840,N,00009.6934,E,0.44,147.21,111015,,,A*6A



  // Do we have a GPRMC message?
  if (strstr(gpsfields[0], "$GPRMC")) {

    time = atof(gpsfields[1]);      // 111837.000
                                    // Status A=active or V=Void.
    latitude = atof(gpsfields[3]);  // 5117.2840
    latNS = gpsfields[4][0];        // N
    longitude = atof(gpsfields[5]); // 00009.6934
    lonEW = gpsfields[6][0];        // E
                                    // Speed over the ground in knots
                                    // Track angle in degrees True
                                    // Date - 11th of October 2015
                                    // Magnetic Variation
                                    // The checksum data, always begins with *

    latitude = DegreeToDecimal(latitude, latNS);
    longitude = DegreeToDecimal(longitude, lonEW);
  }
}


float Pam7qGPS::DegreeToDecimal(float num, byte sign)
{
   // Want to convert DDMM.MMMM to a decimal number DD.DDDDD

   int intpart= (int) num;
   float decpart = num - intpart;

   int degree = (int)(intpart / 100);
   int mins = (int)(intpart % 100);

   if (sign == 'N' || sign == 'E')
   {
     // Return positive degree
     return (degree + (mins + decpart)/60);
   }   
   // Return negative degree
   return -(degree + (mins + decpart)/60);
}

void Pam7qGPS::ParseString(char* msg) {

  // Length of the GPS message
  int len = strlen(msg);
  int n=0;
  int j=0;

  // Loop over the string
  for (int i=0; i= '0' && c <= '9') {
    return c - '0';
  }
  else if (c >= 'A' && c <= 'F') {
    return (c - 'A') + 10;
  }
  else {
    return 0;
  }
}