
#include "gps.h"

volatile nmea_data gps_data;

float gps_latitude()
{
  return(gps_data.lat_dds);
}

