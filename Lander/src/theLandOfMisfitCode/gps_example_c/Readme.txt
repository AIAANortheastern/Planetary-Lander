Welcome to the readme file for Parallax's PAM-7Q GPS Module (#28509) C demo code.  By default, the code expects the GPS module to connected to your Propeller project in the following way:


GPS Module  |  Propeller Project
============|====================
Vdd        <--      3.3 V
txd        -->      P0
rxd        <--      P1
GND        <--      GND

Of course, these defaults can be changed.  All you have to do is change
the defined constants in the "defined constants" section of gps_example.c - GPS_RXIN_PIN, GPS_TXOUT_PIN, and GPS_BAUD - to your desired settings.

Additionally, if you have a 4x20 line serial LCD module (Parallax #27979), you can enable output to the LCD by uncommenting the line "#define ENABLE_LCD" (should be line 22).  If you use an LCD, the program default expects an LCD to be connected to P14 at 9600 baud.  Of course, these settings can be changed as well in the "defined constants" section of gps_example.c.

You will also want to copy the libgps folder into "...Documents\SimpleIDE\Learn\Simple Libraries\Sensor\".  Restart SimpleIDE to make the editor discover the newly added library.

"gps_example.side" and "gps_example.c" can then be placed just about anywhere (though they'll have to be in the same folder).  Example: place both in "...Documents\SimpleIDE\My Projects\"

Open "gps_example.side", and click Run with Terminal.  It may take up to a minute for the GPS to connect to satellites and start sending the Propeller chip position data.  Make sure that your GPS module has a good view of the outside sky - it is almost impossible for the GPS module to get a satellite lock indoors.
