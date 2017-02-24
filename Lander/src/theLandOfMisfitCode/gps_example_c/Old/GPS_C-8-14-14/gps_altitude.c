
#include "gps.h"

volatile nmea_data gps_data;

float gps_altitude()
{
  return(gps_data.altitude);
}
