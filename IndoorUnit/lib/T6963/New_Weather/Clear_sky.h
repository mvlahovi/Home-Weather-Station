//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Clear_sky_h //don't do multiple includes of this file
#define Clear_sky_h

#include <pgmspace.h> //Win-AVR
////#include <progmem.h> //AVR-GCC

#define Clear_skyWIDTH 	22	//Width in pixels
#define Clear_skyHEIGHT 	22	//Height in pixels
#define Clear_skyBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Clear_skybmp[]; //Microchip C18
//extern const unsigned char code Clear_skybmp[]; //Keil
//extern const unsigned char Clear_skybmp[]; //SDCC

const char Clear_skybmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x00,0x00,0x00,
0x00,0x02,0x00,0x00,
0x00,0x02,0x00,0x00,
0x00,0x02,0x01,0x00,
0x0C,0x02,0x03,0x00,
0x06,0x07,0x06,0x00,
0x03,0x1D,0x3C,0x00,
0x01,0x30,0x18,0x00,
0x00,0x20,0x08,0x00,
0x01,0x20,0x0C,0x00,
0x1F,0x00,0x07,0x30,
0x01,0x20,0x0C,0x00,
0x00,0x20,0x08,0x00,
0x00,0x30,0x1C,0x00,
0x01,0x3D,0x36,0x00,
0x03,0x07,0x03,0x00,
0x06,0x02,0x01,0x20,
0x04,0x02,0x00,0x00,
0x00,0x02,0x00,0x00,
0x00,0x02,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00
};

#endif //Clear_sky_h
