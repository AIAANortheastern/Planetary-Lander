/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/

#include "simpletools.h"                      // Include simple tools
#include "gps.h"
#include <unistd.h>                           // included for usleep


#define ENABLE_LCD
#define ENABLE_PC

#define GPS_RXIN_PIN  0
#define GPS_TXOUT_PIN 1
#define GPS_BAUD      9600

#define LCD_PIN       14
#define LCD_BAUD      9600


int main()                                    // main() function
{

  pause(1000);

#ifdef ENABLE_LCD
  serial *lcd = serial_open(LCD_PIN, LCD_PIN, 0, LCD_BAUD);

  writeChar(lcd, 22);     //LCD on, Cursor Off
  writeChar(lcd, 17);     //Backlight On
  writeChar(lcd, 12);     //Clear LCD screen of text
#endif


  //Start the GPS driver
  gps_open(GPS_RXIN_PIN, GPS_TXOUT_PIN, GPS_BAUD);

  pause(100);   //give the drive a moment to start up

  //Infinite loop to print GPS statistics
  while(1)
  {

#ifdef ENABLE_LCD     //only do this section of code if ENABLE_LCD is defined
    writeChar(lcd, 128);
    dprint(lcd,"lat: %f    ", gps_latitude());
    writeChar(lcd, 148);
    dprint(lcd,"lon: %f    ", gps_longitude());
    writeChar(lcd, 168);    
    dprint(lcd, "Deg:%d Alt:%3.1fm   ", (int)gps_heading(), gps_altitude());
    writeChar(lcd, 188);
    dprint(lcd, "Spd:%3.1fmph sats:%d   ", gps_velocity(MPH), gps_satsTracked());
#endif                //end ENABLE_LCD

    
#ifdef ENABLE_PC      //only do this section of code if ENABLE_PC is defined

    //send a "Home" character to the terminal
    print("%c", HOME);

    //begin printing all the GPS information
    print("GPS Fix:          ");
    if(gps_fixValid())
      print("Valid     \n");
    else
      print("Not Valid \n");

    print("Altitude (meters): %3.2f\n", gps_altitude());
    print("Num Satellites:    %d\n", gps_satsTracked());
    print("Compass Heading:   %3.2f\n", gps_heading());
    print("Velocity (knots):  %3.2f\n", gps_velocity(KNOTS));

    print("\nLatitude    Longitude\n");
    print("%f  %f", gps_latitude(), gps_longitude());

    //sleep for 1/4 second
    usleep(250000);

#endif                //end ENABLE_PC

  } //end main loop


} //end main()
