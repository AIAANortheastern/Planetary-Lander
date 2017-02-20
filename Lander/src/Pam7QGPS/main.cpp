#include "TinyGPS.cpp"
#include "Arduino.h"

using namespace std;

int main()
{
	TinyGPS gps_1;

	serial.print(gps_1.altitude());

	return 0;
}

