
#include "gps.h"

volatile int  gps_cog;
volatile int  gps_stopping;
nmea_data gps_data;

void gps_close()
{
  if(gps_cog >= 0)
  {
    memset(&gps_data, 0, sizeof(nmea_data));          //clear the GPS data structure
    gps_stopping = 1;

    while(gps_stopping);

    cogstop(gps_cog);
    gps_cog = -1;
  }
}

