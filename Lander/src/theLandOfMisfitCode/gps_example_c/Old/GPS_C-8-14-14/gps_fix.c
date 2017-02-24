#include "gps.h"

volatile nmea_data gps_data;

int gps_fix()
{
  return(gps_data.fix);
}

