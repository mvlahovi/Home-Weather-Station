//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Snow_h //don't do multiple includes of this file
#define Snow_h

#include <pgmspace.h> //Win-AVR
////#include <progmem.h> //AVR-GCC

#define SnowWIDTH 	22	//Width in pixels
#define SnowHEIGHT 	22	//Height in pixels
#define SnowBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Snowbmp[]; //Microchip C18
//extern const unsigned char code Snowbmp[]; //Keil
//extern const unsigned char Snowbmp[]; //SDCC

const char Snowbmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x03,0x20,0x00,
0x00,0x0E,0x30,0x00,
0x00,0x18,0x18,0x00,
0x00,0x30,0x0F,0x00,
0x01,0x20,0x01,0x20,
0x03,0x00,0x00,0x30,
0x06,0x00,0x00,0x18,
0x04,0x00,0x00,0x08,
0x0C,0x00,0x00,0x08,
0x18,0x00,0x00,0x0C,
0x10,0x10,0x00,0x04,
0x10,0x10,0x00,0x04,
0x11,0x3C,0x00,0x04,
0x18,0x10,0x00,0x0C,
0x06,0x10,0x20,0x18,
0x03,0x00,0x21,0x30,
0x00,0x03,0x38,0x00,
0x00,0x00,0x20,0x00,
0x00,0x00,0x20,0x00,
0x00,0x00,0x00,0x00
};


#endif //Snow_h
