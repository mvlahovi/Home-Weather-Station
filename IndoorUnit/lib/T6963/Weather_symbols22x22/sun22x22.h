//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef sun22x22_h //don't do multiple includes of this file
#define sun22x22_h

#include <pgmspace.h> //Win-AVR
//#include <progmem.h> //AVR-GCC

#define sun22x22WIDTH 	22	//Width in pixels
#define sun22x22HEIGHT 	22	//Height in pixels
#define sun22x22BYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char sun22x22bmp[]; //Microchip C18
//extern const unsigned char code sun22x22bmp[]; //Keil
//extern const unsigned char sun22x22bmp[]; //SDCC

extern const char sun22x22bmp[]; //AVR-GCC, WinAVR

#endif //sun22x22_h
