//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Partly_cloudy_h //don't do multiple includes of this file
#define Partly_cloudy_h

#include <pgmspace.h> //Win-AVR
////#include <progmem.h> //AVR-GCC

#define Partly_cloudyWIDTH 	22	//Width in pixels
#define Partly_cloudyHEIGHT 	22	//Height in pixels
#define Partly_cloudyBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Partly_cloudybmp[]; //Microchip C18
//extern const unsigned char code Partly_cloudybmp[]; //Keil
//extern const unsigned char Partly_cloudybmp[]; //SDCC

const char Partly_cloudybmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x20,0x00,0x00,
0x10,0x20,0x20,0x00,
0x19,0x39,0x20,0x00,
0x0F,0x0E,0x00,0x00,
0x06,0x07,0x38,0x00,
0x04,0x03,0x1F,0x00,
0x3C,0x0E,0x01,0x20,
0x04,0x08,0x00,0x30,
0x0E,0x18,0x00,0x18,
0x0B,0x30,0x00,0x08,
0x1F,0x20,0x00,0x08,
0x1E,0x00,0x00,0x0C,
0x30,0x00,0x00,0x04,
0x20,0x00,0x00,0x04,
0x20,0x00,0x00,0x04,
0x30,0x00,0x00,0x0C,
0x1C,0x00,0x00,0x18,
0x07,0x3F,0x3F,0x30,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00
};

#endif //Partly_cloudy_h
