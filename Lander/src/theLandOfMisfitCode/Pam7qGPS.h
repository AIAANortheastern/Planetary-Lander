#ifndef Pam7qGPS_h
  #define Pam7qGPS_h

  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  // Version of the Library
  #define _LIB_VERSION "1.00"

  // Define byte lengths
  #define _ONE_BYTE 1
  #define _TWO_BYTES 2

  #define MAXLENGTH 120
  #define MAXNUMFIELDS 15
  #define MAXFIELDLENGTH 11

  class Pam7qGPS{
	public:
		static const char *libraryVersion() { return _LIB_VERSION; }

		Pam7qGPS();
		void read(char);
                void CheckGPGGA();
		void CheckGPRMC();
                bool CheckSum(char*);
		int Hex2Dec(char);
                void ParseString(char*);
		float DegreeToDecimal(float, byte);
               
  		float time;
		float latitude;
                float longitude;
                char latNS, lonEW;
                
	private:
		int _IsBusy;

  };

#endif