//#######################################################################
//File generated by BMP2C V1.18 17.01.2008
//hk@holger-klabunde.de
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0 
//#######################################################################
#ifndef Light_sleet_showers_h //don't do multiple includes of this file
#define Light_sleet_showers_h

#include <pgmspace.h> //Win-AVR
////#include <progmem.h> //AVR-GCC

#define Light_sleet_showersWIDTH 	22	//Width in pixels
#define Light_sleet_showersHEIGHT 	22	//Height in pixels
#define Light_sleet_showersBYTEWIDTH 	4	//Width in bytes

//extern const rom unsigned char Light_sleet_showersbmp[]; //Microchip C18
//extern const unsigned char code Light_sleet_showersbmp[]; //Keil
//extern const unsigned char Light_sleet_showersbmp[]; //SDCC

const char Light_sleet_showersbmp PROGMEM []= { //AVR-GCC, WinAVR
0x00,0x10,0x00,0x00,
0x30,0x10,0x20,0x00,
0x19,0x39,0x00,0x00,
0x0E,0x0F,0x00,0x00,
0x04,0x03,0x38,0x00,
0x0C,0x06,0x0F,0x00,
0x38,0x0C,0x01,0x20,
0x08,0x18,0x00,0x30,
0x04,0x30,0x00,0x18,
0x07,0x20,0x00,0x08,
0x0C,0x00,0x00,0x08,
0x18,0x00,0x00,0x0C,
0x30,0x00,0x00,0x04,
0x20,0x00,0x00,0x04,
0x20,0x00,0x00,0x04,
0x30,0x04,0x00,0x0C,
0x1C,0x04,0x00,0x18,
0x07,0x25,0x23,0x30,
0x00,0x04,0x08,0x00,
0x00,0x04,0x1C,0x00,
0x00,0x04,0x08,0x00,
0x00,0x00,0x00,0x00
};

#endif //Light_sleet_showers_h
