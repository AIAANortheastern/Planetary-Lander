 GNU assembler version 2.21 (propeller-elf)
	 using BFD version (propellergcc_v1_0_0_2162) 2.21.
 options passed	: -lmm -cmm -ahdlnsg=cmm/Simple_NMEA_Parser.asm 
 input file    	: C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s
 output file   	: cmm/Simple_NMEA_Parser.o
 target        	: propeller-parallax-elf
 time stamp    	: 

   1              		.text
   2              	.Ltext0
   3              		.global	_gps_start
   4              	_gps_start
   5              	.LFB1
   6              		.file 1 "Simple_NMEA_Parser.c"
   1:Simple_NMEA_Parser.c **** /*
   2:Simple_NMEA_Parser.c **** 
   3:Simple_NMEA_Parser.c ****   Simple_NMEA_Parser.c
   4:Simple_NMEA_Parser.c **** 
   5:Simple_NMEA_Parser.c ****   This library provides basic NMEA parsing capabilities.  It is designed to take raw NMEA strings,
   6:Simple_NMEA_Parser.c ****   parse the data out of them, and make the data available to a parent application.
   7:Simple_NMEA_Parser.c **** 
   8:Simple_NMEA_Parser.c **** */
   9:Simple_NMEA_Parser.c **** 
  10:Simple_NMEA_Parser.c **** #include "simpletools.h"
  11:Simple_NMEA_Parser.c **** #include "fdserial.h"
  12:Simple_NMEA_Parser.c **** 
  13:Simple_NMEA_Parser.c **** #define INBUFF_SIZE    128
  14:Simple_NMEA_Parser.c **** 
  15:Simple_NMEA_Parser.c **** //Type definitions
  16:Simple_NMEA_Parser.c **** typedef unsigned char byte;
  17:Simple_NMEA_Parser.c **** 
  18:Simple_NMEA_Parser.c **** typedef struct nmea_data_s
  19:Simple_NMEA_Parser.c **** {
  20:Simple_NMEA_Parser.c ****   int fix;            //fix quality, 0=invalid, 1=GPS, 2=DGPS, etc...
  21:Simple_NMEA_Parser.c ****   float lat_dds;      //current latitude in decimal degress
  22:Simple_NMEA_Parser.c ****   float lon_dds;      //current longitude in decimal degrees
  23:Simple_NMEA_Parser.c ****   int sats_tracked;   //current number of satellites tracked by the GPS
  24:Simple_NMEA_Parser.c ****   int altitude;       //current altitude, in meters
  25:Simple_NMEA_Parser.c ****   float heading;      //current direction of travel, in degrees
  26:Simple_NMEA_Parser.c ****   float velocity;     //current speed if travel, in knots
  27:Simple_NMEA_Parser.c ****   int date;           //current date, raw format
  28:Simple_NMEA_Parser.c ****   int time;           //current UTC time, raw format
  29:Simple_NMEA_Parser.c ****   int mag_var;        //current magnetic variation
  30:Simple_NMEA_Parser.c **** 
  31:Simple_NMEA_Parser.c **** } nmea_data;
  32:Simple_NMEA_Parser.c **** 
  33:Simple_NMEA_Parser.c **** 
  34:Simple_NMEA_Parser.c **** //Function Prototypes
  35:Simple_NMEA_Parser.c **** void gps_run(void *par);
  36:Simple_NMEA_Parser.c **** void ParseGGA(int, byte *);
  37:Simple_NMEA_Parser.c **** void ParseRMC(int, byte *);
  38:Simple_NMEA_Parser.c **** 
  39:Simple_NMEA_Parser.c **** 
  40:Simple_NMEA_Parser.c **** //Global variable declarations
  41:Simple_NMEA_Parser.c **** int _gps_rx_pin, _gps_baud;
  42:Simple_NMEA_Parser.c **** nmea_data gps_data;
  43:Simple_NMEA_Parser.c **** 
  44:Simple_NMEA_Parser.c **** byte tempBuff[16];
  45:Simple_NMEA_Parser.c **** 
  46:Simple_NMEA_Parser.c **** 
  47:Simple_NMEA_Parser.c **** unsigned char *gps_start(int gps_rx_pin, int gps_baud)
  48:Simple_NMEA_Parser.c **** {
   7              		.loc 1 48 0
   8              	.LVL0
   9 0000 031F     		lpushm	#16+15
  10              	.LCFI0
  49:Simple_NMEA_Parser.c **** 
  50:Simple_NMEA_Parser.c ****   _gps_rx_pin = gps_rx_pin;
  11              		.loc 1 50 0
  12 0002 670000   		mviw	r7,#__gps_rx_pin
  13 0005 107F     		wrlong	r0, r7
  51:Simple_NMEA_Parser.c ****   _gps_baud = gps_baud;
  14              		.loc 1 51 0
  15 0007 670000   		mviw	r7,#__gps_baud
  52:Simple_NMEA_Parser.c **** 
  53:Simple_NMEA_Parser.c **** 
  54:Simple_NMEA_Parser.c **** 
  55:Simple_NMEA_Parser.c ****   cog_run(&gps_run, 50);
  16              		.loc 1 55 0
  17 000a 600000   		mviw	r0,#_gps_run
  18              	.LVL1
  51:Simple_NMEA_Parser.c ****   _gps_baud = gps_baud;
  19              		.loc 1 51 0
  20 000d 117F     		wrlong	r1, r7
  21              		.loc 1 55 0
  22 000f A132     		mov	r1, #50
  23              	.LVL2
  24 0011 060000   		lcall	#_cog_run
  25              	.LVL3
  56:Simple_NMEA_Parser.c **** 
  57:Simple_NMEA_Parser.c ****   return tempBuff;
  58:Simple_NMEA_Parser.c **** 
  59:Simple_NMEA_Parser.c **** }
  26              		.loc 1 59 0
  27 0014 600000   		mviw	r0,#_tempBuff
  28 0017 051F     		lpopret	#16+15
  29              	.LFE1
  30              		.global	_ParseGGA
  31              	_ParseGGA
  32              	.LFB3
  60:Simple_NMEA_Parser.c **** 
  61:Simple_NMEA_Parser.c **** void gps_run(void *par)
  62:Simple_NMEA_Parser.c **** {
  63:Simple_NMEA_Parser.c **** 
  64:Simple_NMEA_Parser.c ****   const char gprmc_str[] = "GPRMC";
  65:Simple_NMEA_Parser.c ****   const char gpgga_str[] = "GPGGA";
  66:Simple_NMEA_Parser.c ****   fdserial *gps_ser;
  67:Simple_NMEA_Parser.c ****   byte ch;
  68:Simple_NMEA_Parser.c ****   byte inBuff[INBUFF_SIZE];
  69:Simple_NMEA_Parser.c ****   byte tempBuff[16];
  70:Simple_NMEA_Parser.c ****   int idx, j;
  71:Simple_NMEA_Parser.c **** 
  72:Simple_NMEA_Parser.c **** 
  73:Simple_NMEA_Parser.c **** 
  74:Simple_NMEA_Parser.c ****   memset(inBuff, 0, INBUFF_SIZE);
  75:Simple_NMEA_Parser.c ****   memset(tempBuff, 0, 16);
  76:Simple_NMEA_Parser.c **** 
  77:Simple_NMEA_Parser.c ****   low(26);
  78:Simple_NMEA_Parser.c **** 
  79:Simple_NMEA_Parser.c ****   gps_ser = fdserial_open(_gps_rx_pin, 1, 0, _gps_baud);
  80:Simple_NMEA_Parser.c ****   for(;;)
  81:Simple_NMEA_Parser.c ****   {
  82:Simple_NMEA_Parser.c ****     ch = fdserial_rxChar(gps_ser);
  83:Simple_NMEA_Parser.c ****     
  84:Simple_NMEA_Parser.c ****     if(ch != '$')
  85:Simple_NMEA_Parser.c ****       continue;
  86:Simple_NMEA_Parser.c **** 
  87:Simple_NMEA_Parser.c **** //    memset(inBuff, 0, INBUFF_SIZE);
  88:Simple_NMEA_Parser.c ****     idx = 0;
  89:Simple_NMEA_Parser.c ****     do
  90:Simple_NMEA_Parser.c ****     {
  91:Simple_NMEA_Parser.c ****       ch = fdserial_rxChar(gps_ser);
  92:Simple_NMEA_Parser.c ****       inBuff[idx++] = ch;
  93:Simple_NMEA_Parser.c ****       
  94:Simple_NMEA_Parser.c ****     }while(ch != 13);
  95:Simple_NMEA_Parser.c **** 
  96:Simple_NMEA_Parser.c ****     inBuff[idx] = 0;      //null terminate
  97:Simple_NMEA_Parser.c **** 
  98:Simple_NMEA_Parser.c ****     //parse out the first field
  99:Simple_NMEA_Parser.c ****     idx=0;   //reset buffer offset pointer
 100:Simple_NMEA_Parser.c ****     for(j=0; inBuff[idx]!=',' || j>=15; idx++,j++)
 101:Simple_NMEA_Parser.c ****       tempBuff[j] = inBuff[idx];
 102:Simple_NMEA_Parser.c **** 
 103:Simple_NMEA_Parser.c ****     tempBuff[j]=0;      //null terminate
 104:Simple_NMEA_Parser.c **** 
 105:Simple_NMEA_Parser.c ****     if(strcmp(gprmc_str, tempBuff)==0)
 106:Simple_NMEA_Parser.c ****       ParseRMC(idx, inBuff);
 107:Simple_NMEA_Parser.c ****     if(strcmp(gpgga_str, tempBuff)==0)  
 108:Simple_NMEA_Parser.c ****       ParseGGA(idx, inBuff);
 109:Simple_NMEA_Parser.c ****     
 110:Simple_NMEA_Parser.c **** 
 111:Simple_NMEA_Parser.c **** 
 112:Simple_NMEA_Parser.c ****   }
 113:Simple_NMEA_Parser.c **** 
 114:Simple_NMEA_Parser.c **** }
 115:Simple_NMEA_Parser.c **** 
 116:Simple_NMEA_Parser.c **** void ParseGGA(int idx, byte *buff)
 117:Simple_NMEA_Parser.c **** {
  33              		.loc 1 117 0
  34              	.LVL4
 118:Simple_NMEA_Parser.c ****   int j;
 119:Simple_NMEA_Parser.c **** 
 120:Simple_NMEA_Parser.c ****   idx++;
 121:Simple_NMEA_Parser.c ****   for(j=0; buff[idx]!=',' || j>=15; idx++,j++)
  35              		.loc 1 121 0
  36 0019 B7       		mov	r7, #0
 116:Simple_NMEA_Parser.c **** void ParseGGA(int idx, byte *buff)
  37              		.loc 1 116 0
  38 001a 1100     		add	r1, r0
  39              	.LVL5
  40 001c 650000   		mviw	r5,#_tempBuff
  41              		.loc 1 121 0
  42 001f 7F07     		brs	#.L3
  43              	.LVL6
  44              	.L4
 116:Simple_NMEA_Parser.c **** void ParseGGA(int idx, byte *buff)
  45              		.loc 1 116 0 discriminator 2
  46 0021 D44570   		xmov	r4,r5 add r4,r7
  47              		.loc 1 121 0 discriminator 2
  48 0024 2710     		add	r7, #1
  49              	.LVL7
 122:Simple_NMEA_Parser.c ****     tempBuff[j] = buff[idx];
  50              		.loc 1 122 0 discriminator 2
  51 0026 164E     		wrbyte	r6, r4
  52              	.LVL8
  53              	.L3
 116:Simple_NMEA_Parser.c **** void ParseGGA(int idx, byte *buff)
  54              		.loc 1 116 0 discriminator 1
  55 0028 D66170   		xmov	r6,r1 add r6,r7
  56 002b 2610     		add	r6, #1
 121:Simple_NMEA_Parser.c ****   for(j=0; buff[idx]!=',' || j>=15; idx++,j++)
  57              		.loc 1 121 0 discriminator 1
  58 002d 166C     		rdbyte	r6, r6
  59 002f 362C20   		cmps	r6, #44 wz,wc
  60 0032 75ED     		IF_NE	brs	#.L4
  61 0034 27F2     		cmps	r7, #15 wz,wc
  62 0036 73E9     		IF_AE	brs	#.L4
 123:Simple_NMEA_Parser.c **** 
 124:Simple_NMEA_Parser.c ****   tempBuff[j]=0;      //null terminate
  63              		.loc 1 124 0
  64 0038 660000   		mviw	r6,#_tempBuff
  65 003b 1670     		add	r6, r7
  66 003d B7       		mov	r7, #0
  67              	.LVL9
  68 003e 176E     		wrbyte	r7, r6
 125:Simple_NMEA_Parser.c **** 
 126:Simple_NMEA_Parser.c ****     
 127:Simple_NMEA_Parser.c ****   
 128:Simple_NMEA_Parser.c ****   
 129:Simple_NMEA_Parser.c **** }
  69              		.loc 1 129 0
  70 0040 02       		lret
  71              	.LFE3
  72              		.global	_gps_run
  73              	_gps_run
  74              	.LFB2
  62:Simple_NMEA_Parser.c **** {
  75              		.loc 1 62 0
  76              	.LVL10
  77 0041 034C     		lpushm	#(4<<4)+12
  78              	.LCFI1
  79 0043 F3982084 		sub	sp, #152
  80              	.LCFI2
  65:Simple_NMEA_Parser.c ****   const char gpgga_str[] = "GPGGA";
  81              		.loc 1 65 0
  82 0047 56475047 		mvi	r6,#1195855943
  82      47
  74:Simple_NMEA_Parser.c ****   memset(inBuff, 0, INBUFF_SIZE);
  83              		.loc 1 74 0
  84 004c B1       		mov	r1, #0
  65:Simple_NMEA_Parser.c ****   const char gpgga_str[] = "GPGGA";
  85              		.loc 1 65 0
  86 004d C704     		leasp r7,#4
  74:Simple_NMEA_Parser.c ****   memset(inBuff, 0, INBUFF_SIZE);
  87              		.loc 1 74 0
  88 004f CD18     		leasp r13,#24
  65:Simple_NMEA_Parser.c ****   const char gpgga_str[] = "GPGGA";
  89              		.loc 1 65 0
  90 0051 F0100C08 		wrlong	r6, sp
  91 0055 A641     		mov	r6, #65
  74:Simple_NMEA_Parser.c ****   memset(inBuff, 0, INBUFF_SIZE);
  92              		.loc 1 74 0
  93 0057 A280     		mov	r2, #128
  65:Simple_NMEA_Parser.c ****   const char gpgga_str[] = "GPGGA";
  94              		.loc 1 65 0
  95 0059 F0070C04 		wrword	r6, r7
  74:Simple_NMEA_Parser.c ****   memset(inBuff, 0, INBUFF_SIZE);
  96              		.loc 1 74 0
  97 005d 0A0D     		mov	r0, r13
  98              	.LVL11
  99 005f 060000   		lcall	#_memset
  75:Simple_NMEA_Parser.c ****   memset(tempBuff, 0, 16);
 100              		.loc 1 75 0
 101 0062 B1       		mov	r1, #0
 102 0063 A210     		mov	r2, #16
 103 0065 C008     		leasp r0,#8
 104 0067 060000   		lcall	#_memset
  77:Simple_NMEA_Parser.c ****   low(26);
 105              		.loc 1 77 0
 106 006a A01A     		mov	r0, #26
 107 006c 060000   		lcall	#_low
  79:Simple_NMEA_Parser.c ****   gps_ser = fdserial_open(_gps_rx_pin, 1, 0, _gps_baud);
 108              		.loc 1 79 0
 109 006f 670000   		mviw	r7,#__gps_rx_pin
 110 0072 A101     		mov	r1, #1
 111 0074 B2       		mov	r2, #0
 112 0075 107D     		rdlong	r0, r7
 113 0077 670000   		mviw	r7,#__gps_baud
 114 007a 137D     		rdlong	r3, r7
 115 007c 060000   		lcall	#_fdserial_open
 116 007f 0AC0     		mov	r12, r0
 117              	.LVL12
 118              	.L15
  82:Simple_NMEA_Parser.c ****     ch = fdserial_rxChar(gps_ser);
 119              		.loc 1 82 0
 120 0081 0A0C     		mov	r0, r12
 121 0083 060000   		lcall	#_fdserial_rxChar
 122              	.LVL13
  84:Simple_NMEA_Parser.c ****     if(ch != '$')
 123              		.loc 1 84 0
 124 0086 30FF40   		and	r0,#255
 125              	.LVL14
 126 0089 302420   		cmps	r0, #36 wz,wc
 127 008c 75F3     		IF_NE	brs	#.L15
 128 008e BE       		mov	r14, #0
 129              	.L7
 130              	.LVL15
  91:Simple_NMEA_Parser.c ****       ch = fdserial_rxChar(gps_ser);
 131              		.loc 1 91 0 discriminator 1
 132 008f 0A0C     		mov	r0, r12
 133 0091 060000   		lcall	#_fdserial_rxChar
  61:Simple_NMEA_Parser.c **** void gps_run(void *par)
 134              		.loc 1 61 0 discriminator 1
 135 0094 0A7D     		mov	r7, r13
  91:Simple_NMEA_Parser.c ****       ch = fdserial_rxChar(gps_ser);
 136              		.loc 1 91 0 discriminator 1
 137 0096 30FF40   		and	r0,#255
 138              	.LVL16
  61:Simple_NMEA_Parser.c **** void gps_run(void *par)
 139              		.loc 1 61 0 discriminator 1
 140 0099 17E0     		add	r7, r14
  94:Simple_NMEA_Parser.c ****     }while(ch != 13);
 141              		.loc 1 94 0 discriminator 1
 142 009b 20D2     		cmps	r0, #13 wz,wc
  92:Simple_NMEA_Parser.c ****       inBuff[idx++] = ch;
 143              		.loc 1 92 0 discriminator 1
 144 009d 2E10     		add	r14, #1
 145              	.LVL17
 146 009f 107E     		wrbyte	r0, r7
  94:Simple_NMEA_Parser.c ****     }while(ch != 13);
 147              		.loc 1 94 0 discriminator 1
 148 00a1 75EC     		IF_NE	brs	#.L7
  96:Simple_NMEA_Parser.c ****     inBuff[idx] = 0;      //null terminate
 149              		.loc 1 96 0
 150 00a3 1ED0     		add	r14, r13
 151              	.LVL18
 152 00a5 B7       		mov	r7, #0
 153 00a6 17EE     		wrbyte	r7, r14
 154              	.LVL19
  99:Simple_NMEA_Parser.c ****     idx=0;   //reset buffer offset pointer
 155              		.loc 1 99 0
 156 00a8 BE       		mov	r14, #0
 100:Simple_NMEA_Parser.c ****     for(j=0; inBuff[idx]!=',' || j>=15; idx++,j++)
 157              		.loc 1 100 0
 158 00a9 7F08     		brs	#.L8
 159              	.LVL20
 160              	.L9
  61:Simple_NMEA_Parser.c **** void gps_run(void *par)
 161              		.loc 1 61 0 discriminator 2
 162 00ab C608     		leasp r6,#8
 163 00ad 16E0     		add	r6, r14
 100:Simple_NMEA_Parser.c ****     for(j=0; inBuff[idx]!=',' || j>=15; idx++,j++)
 164              		.loc 1 100 0 discriminator 2
 165 00af 2E10     		add	r14, #1
 166              	.LVL21
 101:Simple_NMEA_Parser.c ****       tempBuff[j] = inBuff[idx];
 167              		.loc 1 101 0 discriminator 2
 168 00b1 176E     		wrbyte	r7, r6
 169              	.LVL22
 170              	.L8
  61:Simple_NMEA_Parser.c **** void gps_run(void *par)
 171              		.loc 1 61 0 discriminator 1
 172 00b3 D77DE0   		xmov	r7,r13 add r7,r14
 100:Simple_NMEA_Parser.c ****     for(j=0; inBuff[idx]!=',' || j>=15; idx++,j++)
 173              		.loc 1 100 0 discriminator 1
 174 00b6 177C     		rdbyte	r7, r7
 175 00b8 372C20   		cmps	r7, #44 wz,wc
 176 00bb 75EE     		IF_NE	brs	#.L9
 177 00bd 2EF2     		cmps	r14, #15 wz,wc
 178 00bf 73EA     		IF_AE	brs	#.L9
 103:Simple_NMEA_Parser.c ****     tempBuff[j]=0;      //null terminate
 179              		.loc 1 103 0
 180 00c1 C708     		leasp r7,#8
 181 00c3 17E0     		add	r7, r14
 182 00c5 B6       		mov	r6, #0
 107:Simple_NMEA_Parser.c ****     if(strcmp(gpgga_str, tempBuff)==0)  
 183              		.loc 1 107 0
 184 00c6 F21000A0 		mov	r0, sp
 185              	.LVL23
 186 00ca C108     		leasp r1,#8
 103:Simple_NMEA_Parser.c ****     tempBuff[j]=0;      //null terminate
 187              		.loc 1 103 0
 188 00cc 167E     		wrbyte	r6, r7
 107:Simple_NMEA_Parser.c ****     if(strcmp(gpgga_str, tempBuff)==0)  
 189              		.loc 1 107 0
 190 00ce 060000   		lcall	#_strcmp
 191 00d1 2002     		cmps	r0, #0 wz,wc
 192 00d3 450000   		IF_NE	brw	#.L15
 108:Simple_NMEA_Parser.c ****       ParseGGA(idx, inBuff);
 193              		.loc 1 108 0
 194 00d6 0B0E1D   		xmov	r0,r14 mov r1,r13
 195 00d9 060000   		lcall	#_ParseGGA
 196 00dc 4F0000   		brw	#.L15
 197              	.LFE2
 198              		.global	_ParseRMC
 199              	_ParseRMC
 200              	.LFB4
 130:Simple_NMEA_Parser.c **** 
 131:Simple_NMEA_Parser.c **** void ParseRMC(int idx, byte *buff)
 132:Simple_NMEA_Parser.c **** {
 201              		.loc 1 132 0
 202              	.LVL24
 133:Simple_NMEA_Parser.c ****   
 134:Simple_NMEA_Parser.c **** }
 203              		.loc 1 134 0
 204 00df 02       		lret
 205              	.LFE4
 206              		.comm	_tempBuff,16,4
 207              		.comm	_gps_data,40,4
 208              		.comm	__gps_baud,4,4
 209              		.comm	__gps_rx_pin,4,4
 277              	.Letext0
 278              		.file 2 "C:/Users/dharris/Documents/SimpleIDE/Learn/Simple Libraries/TextDevices/libsimpletext/sim
 279              		.file 3 "C:/Users/dharris/Documents/SimpleIDE/Learn/Simple Libraries/TextDevices/libfdserial/fdser
DEFINED SYMBOLS
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:2      .text:00000000 .Ltext0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:4      .text:00000000 _gps_start
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:5      .text:00000000 .LFB1
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:9      .text:00000000 L0
                            *COM*:00000004 __gps_rx_pin
                            *COM*:00000004 __gps_baud
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:73     .text:00000041 _gps_run
                            *COM*:00000010 _tempBuff
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:29     .text:00000019 .LFE1
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:31     .text:00000019 _ParseGGA
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:32     .text:00000019 .LFB3
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:71     .text:00000041 .LFE3
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:74     .text:00000041 .LFB2
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:118    .text:00000081 .L15
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:197    .text:000000df .LFE2
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:199    .text:000000df _ParseRMC
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:200    .text:000000df .LFB4
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:205    .text:000000e0 .LFE4
                            *COM*:00000028 _gps_data
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:211    .debug_frame:00000000 .Lframe0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:277    .text:000000e0 .Letext0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:281    .debug_info:00000000 .Ldebug_info0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:797    .debug_abbrev:00000000 .Ldebug_abbrev0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1206   .debug_loc:00000000 .LLST0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1218   .debug_loc:00000022 .LLST1
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1230   .debug_loc:00000044 .LLST2
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1239   .debug_loc:00000059 .LLST3
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1246   .debug_loc:0000006c .LLST4
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1268   .debug_loc:000000a3 .LLST5
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1286   .debug_loc:000000d0 .LLST6
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1293   .debug_loc:000000e3 .LLST7
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1304   .debug_loc:00000101 .LLST8
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1320   .debug_loc:0000012b .LLST9
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1355   .debug_line:00000000 .Ldebug_line0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1357   .debug_str:00000000 .LASF2
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1359   .debug_str:0000000a .LASF0
C:\Users\dharris\AppData\Local\Temp\ccGb5XHD.s:1361   .debug_str:00000013 .LASF1

UNDEFINED SYMBOLS
_cog_run
_memset
_low
_fdserial_open
_fdserial_rxChar
_strcmp
