#include <Pam7qGPS.h>
 
    //A GPS library
    Pam7qGPS gps;

    void setup()
    {
      // Update the User
      Serial.begin(9600);

      // Connect to the GPS device
      Serial1.begin(9600);

      // Print the Library version
      Serial.println(gps.libraryVersion());
    }

    void loop()
    {
      // No actions within the loop.

      Serial.print(gps.latitude,8);
      Serial.print(", ");
      Serial.println(gps.longitude,8);
      
    }

    void serialEvent1()
    {
      //This event is called when Serial1 receives new bytes
      while (Serial1.available()) 
      {
        // Read the new byte:
        gps.read((char)Serial1.read());
      }
    }