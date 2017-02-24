#include "gps.h"

volatile nmea_data gps_data;

float gps_longitude()
{
  return(gps_data.lon_dds);
}
