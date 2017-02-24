
#include "gps.h"

volatile nmea_data gps_data;

int gps_fixValid()
{
  return(gps_data.fix_valid);
}
