/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "Simple_NMEA_Parser.h"
#include "fdserial.h"

#include <unistd.h>


int main()                                    // Main function
{
  // Add startup code here.

  serial *lcd = serial_open(14,14,0,9600);

  writeChar(lcd, 22);
  writeChar(lcd, 17);
  writeChar(lcd, 12);

  pause(1000);

  print("%d  \n", gps_open(0, 1, 9600));
 
  
  pause(100);

  print("%d  \n", gps_changeBaud(9600));


  while(1)
  {
    writeChar(lcd, 128);
    dprint(lcd,"lat: %f  ", gps_latitude());
    writeChar(lcd, 148);
    dprint(lcd,"lon: %f  ", gps_longitude());
    writeChar(lcd, 168);    
    dprint(lcd, "Deg:%d Alt:%3.1f ", (int)gps_heading(), gps_altitude());
    writeChar(lcd, 188);
    dprint(lcd, "Spd: %3.1f mph  %d ", gps_velocity(1), gps_numTrackedSats());
    
/*
    // Add main loop code here.
    print("%c", HOME);

    print("GPS Fix:          ");
    if(GetFixValid())
      print("Valid     \n");
    else
      print("Not Valid \n");

    print("Altitude:         %3.2f\n", GetAltitude());
    print("Num Satellites:   %d\n", GetNumTrackedSats());
    print("Compass Heading:  %3.2f\n", GetHeading());
    print("Velocity (knots): %3.2f\n", GetVelocity());

    print("\nLatitude    Longitude\n");
    print("%f  %f", GetLatitude(), GetLongitude());
    
    usleep(250000);
    print("\n");
 */   
  }  


}
