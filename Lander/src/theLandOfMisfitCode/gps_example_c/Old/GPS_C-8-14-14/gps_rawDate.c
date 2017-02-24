#include "gps.h"

volatile nmea_data gps_data;

int gps_rawDate()
{
  return(gps_data.date);
}

