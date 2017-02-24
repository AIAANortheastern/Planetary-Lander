#include "gps.h"

volatile nmea_data gps_data;

float gps_magneticVariation()
{
  return(gps_data.mag_var);
}

