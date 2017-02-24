
#include "gps.h"

volatile nmea_data gps_data;

int gps_satsTracked()
{
  return(gps_data.sats_tracked);
}

