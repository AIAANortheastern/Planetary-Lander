#include "gps.h"

volatile nmea_data gps_data;

int gps_rawTime()
{
  return(gps_data.time);
}

