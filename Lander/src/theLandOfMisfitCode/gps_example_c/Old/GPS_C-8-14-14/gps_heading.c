
#include "gps.h"

volatile nmea_data gps_data;

float gps_heading()
{
  return(gps_data.heading);
}
