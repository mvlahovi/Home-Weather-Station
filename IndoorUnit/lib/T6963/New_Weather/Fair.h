//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Fair_h //don't do multiple includes of this file
#define Fair_h

#include <pgmspace.h> //Win-AVR
////#include <progmem.h> //AVR-GCC

#define FairWIDTH 	22	//Width in pixels
#define FairHEIGHT 	22	//Height in pixels
#define FairBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Fairbmp[]; //Microchip C18
//extern const unsigned char code Fairbmp[]; //Keil
//extern const unsigned char Fairbmp[]; //SDCC

const char Fairbmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x20,0x00,0x00,
0x10,0x20,0x20,0x00,
0x19,0x39,0x20,0x00,
0x0F,0x0E,0x00,0x00,
0x06,0x02,0x00,0x00,
0x04,0x03,0x00,0x00,
0x3C,0x01,0x39,0x20,
0x04,0x03,0x03,0x10,
0x0E,0x02,0x04,0x18,
0x1B,0x0F,0x28,0x08,
0x11,0x39,0x30,0x08,
0x00,0x27,0x00,0x0C,
0x00,0x2C,0x00,0x04,
0x00,0x38,0x00,0x04,
0x00,0x20,0x00,0x04,
0x01,0x00,0x00,0x0C,
0x00,0x20,0x00,0x18,
0x00,0x1F,0x3F,0x30,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00
};

#endif //Fair_h
