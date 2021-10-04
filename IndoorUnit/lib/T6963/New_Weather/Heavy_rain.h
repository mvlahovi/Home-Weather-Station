//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Heavy_rain_h //don't do multiple includes of this file
#define Heavy_rain_h

#include <pgmspace.h> //Win-AVR
////#include <progmem.h> //AVR-GCC

#define Heavy_rainWIDTH 	22	//Width in pixels
#define Heavy_rainHEIGHT 	22	//Height in pixels
#define Heavy_rainBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Heavy_rainbmp[]; //Microchip C18
//extern const unsigned char code Heavy_rainbmp[]; //Keil
//extern const unsigned char Heavy_rainbmp[]; //SDCC

const char Heavy_rainbmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x03,0x20,0x00,
0x00,0x0E,0x30,0x00,
0x00,0x38,0x18,0x00,
0x00,0x20,0x0F,0x00,
0x00,0x20,0x01,0x20,
0x03,0x20,0x00,0x30,
0x06,0x00,0x00,0x18,
0x04,0x00,0x00,0x08,
0x1C,0x00,0x00,0x08,
0x10,0x00,0x00,0x0C,
0x30,0x00,0x00,0x04,
0x20,0x01,0x00,0x04,
0x20,0x09,0x08,0x04,
0x20,0x09,0x08,0x04,
0x1C,0x09,0x08,0x18,
0x07,0x29,0x0B,0x30,
0x00,0x29,0x0A,0x00,
0x00,0x09,0x08,0x00,
0x00,0x08,0x08,0x00,
0x00,0x00,0x00,0x00
};

#endif //Heavy_rain_h
