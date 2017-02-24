#include "gps.h"

volatile int  gps_cog;
volatile int  gps_stopping;
int  gps_stack[100];
int _gps_rx_pin, _gps_tx_pin, _gps_baud;

nmea_data gps_data;

void gps_run(void *par);

int gps_open(int gpsSin, int gpsSout, int gps_baud)   // Open reader, start reading
{

  gps_stopping = 0;
  gps_cog = cogstart(&gps_run, NULL, gps_stack, sizeof(gps_stack));

  if(gps_cog < 0)
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

  return(gps_cog < 0 ? GPS_FALSE:GPS_TRUE);
}

