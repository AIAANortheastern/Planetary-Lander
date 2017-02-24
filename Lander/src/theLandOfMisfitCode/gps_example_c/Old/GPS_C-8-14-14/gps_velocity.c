

#include "gps.h"

volatile nmea_data gps_data;

float gps_velocity(int unit_type)
{
/*
  Returns the velocity measurement from the GPS, in the desired predefined unit type.
*/
  float vel = gps_data.velocity;

  switch(unit_type)
  {
    case KNOTS:
      break;
    case MPH:
      //Conversion, knots to miles per hour (MPH).
      //1 Knot = 1.15078 MPH
      vel *= 1.15078;
      break;
    case KPH:
      //Conversion, knots to kilometers per hour (KPH).
      //1 Knot = 1.852 KPH
      vel *= 1.852;
      break;
    case MPS:
      //Conversion, knots to meters per second (mps).
      //1 Knot = .5144444 m/s
      vel *= 0.514444444444444;
      break;
    default:
      //invalid type specifier
      vel = -1;
  }
  return(vel);
}

