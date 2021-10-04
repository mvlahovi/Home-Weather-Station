//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Light_rain_showers_n_h //don't do multiple includes of this file
#define Light_rain_showers_n_h

#include <pgmspace.h> //Win-AVR
//#include <progmem.h> //AVR-GCC

#define Light_rain_showers_nWIDTH 	22	//Width in pixels
#define Light_rain_showers_nHEIGHT 	22	//Height in pixels
#define Light_rain_showers_nBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Light_rain_showers_nbmp[]; //Microchip C18
//extern const unsigned char code Light_rain_showers_nbmp[]; //Keil
//extern const unsigned char Light_rain_showers_nbmp[]; //SDCC

const char Light_rain_showers_nbmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x00,0x00,0x00,
0x03,0x38,0x00,0x00,
0x01,0x06,0x00,0x00,
0x00,0x21,0x00,0x00,
0x00,0x21,0x38,0x00,
0x00,0x23,0x2F,0x00,
0x00,0x22,0x01,0x20,
0x01,0x06,0x00,0x30,
0x21,0x0C,0x00,0x18,
0x3E,0x18,0x00,0x08,
0x20,0x10,0x00,0x08,
0x13,0x20,0x00,0x0C,
0x3C,0x00,0x00,0x04,
0x20,0x00,0x00,0x04,
0x20,0x00,0x00,0x04,
0x30,0x04,0x00,0x0C,
0x1C,0x04,0x00,0x18,
0x07,0x24,0x3F,0x30,
0x00,0x04,0x00,0x00,
0x00,0x04,0x00,0x00,
0x00,0x04,0x00,0x00,
0x00,0x00,0x00,0x00
};

#endif //Light_rain_showers_n_h
