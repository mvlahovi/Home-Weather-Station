//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Fair_n_h //don't do multiple includes of this file
#define Fair_n_h

#include <pgmspace.h> //Win-AVR
////#include <progmem.h> //AVR-GCC

#define Fair_nWIDTH 	22	//Width in pixels
#define Fair_nHEIGHT 	22	//Height in pixels
#define Fair_nBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Fair_nbmp[]; //Microchip C18
//extern const unsigned char code Fair_nbmp[]; //Keil
//extern const unsigned char Fair_nbmp[]; //SDCC

const char Fair_nbmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x00,0x00,0x00,
0x01,0x3C,0x00,0x00,
0x00,0x13,0x00,0x00,
0x00,0x08,0x20,0x00,
0x00,0x08,0x10,0x00,
0x00,0x04,0x10,0x00,
0x00,0x04,0x09,0x20,
0x00,0x04,0x0B,0x10,
0x00,0x04,0x0C,0x18,
0x00,0x08,0x08,0x08,
0x00,0x09,0x38,0x08,
0x00,0x37,0x00,0x0C,
0x07,0x0C,0x00,0x04,
0x0C,0x18,0x00,0x04,
0x07,0x20,0x00,0x04,
0x01,0x20,0x00,0x0C,
0x00,0x20,0x00,0x18,
0x00,0x1F,0x3F,0x30,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00
};


#endif //Fair_n_h
