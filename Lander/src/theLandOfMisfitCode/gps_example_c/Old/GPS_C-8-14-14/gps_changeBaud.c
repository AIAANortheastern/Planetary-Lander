
#include "gps.h"

volatile int _gps_rx_pin, _gps_tx_pin;

int gps_changeBaud(int newBaudRate)
{
  gps_close();
  pause(50);
  return(gps_open(_gps_rx_pin, _gps_tx_pin, newBaudRate));
}
