#ifndef _T6963C_H_
#define _T6963C_H_

/* ----------------------------------------------------------
 * Program to control a T6963C-based 240x64 pixel LCD display
 * 
 * 
 *
 * Written by John P. Beale May 3-4, 1997  beale@best.com
 *
 * Converted by Nico Sachs Feb. 2006 for AVR-GCC
 *
 *  Based on information from Steve Lawther,
 *  "Writing Software for T6963C based Graphic LCDs", 1997 which is at
 *  http://ourworld.compuserve.com/homepages/steve_lawther/t6963c.pdf
 *
 *  and the Toshiba T6963C data sheet, also on Steve's WWW page
 *
 *  and info at: http://www.citilink.com/~jsampson/lcdindex.htm
 *               http://www.cs.colostate.edu/~hirsch/LCD.html
 *               http://www.hantronix.com/
 * ----------------------------------------------------------
 */
 
/* Hinzugefügt: 
 *		- Funktionalität zur Unterstützung des Font Select Signals
 * Änderungen: 
 *		- glcd_clrpixel() und glcd_setpixel() auskommentiert, da Funktion bereits durch glcd_pixel() gegeben
 *		- stdlib.h, string.h nicht mehr inkludiert, da Librarys nicht genutzt werden
 *
 * weitere Datenblätter/Infos: 	http://forum.lcdinfo.com/viewtopic.php?t=1187
 *								http://www.lcd-module.de/eng/pdf/zubehoer/t6963.pdf
 *								http://www.taylec.co.uk/pdfs/T6963c.pdf
 *								http://www.grote.net/bascom/files/writing_software_to_control_t6963c_based_displays.pdf (!!!)
 *
 * 3/2006 Florian Schäffer, http://www.blafusel.de
 */

//#include "main.h"

/*
 *  FG                (1)  frame ground
 *  GND               (2)  ground 
 *  VDD               (3)  LCD logic supply  +5 DC
 *  VO                (4)  Contrast
 *  /WR               (5)  <--> 
 *  /RD               (6)  <--> 
 *  /CE or /CS        (7)  <--> 
 *  CD or RS          (8)  <-->
 *  /RST              (9)  <-->   

 *  D0                (10) <--> 
 *  D1                (11) <--> 
 *  D2                (12) <--> 
 *  D3                (13) <--> 
 *  D4                (14) <--> 
 *  D5                (15) <--> 
 *  D6                (16) <--> 
 *  D7                (17) <--> 

 *  FS                (18)           font select       HI : narrow            LOW : Wide   ( normal )
                                                       40 with 6x8 font       30 with 8x8 font 
 *  Vout              (19)           Variable Resistor (~5K Ohm) with GND and centre with VO ( Pin 4 ). Used for contrast adjustment. 
 *  BLA               (20)           Back light + (+5V DC)
 *  BLK               (21)           Back light - (GND)
 *  DSPON             (22)           I don't Know   but i think means display ON or ...
 */

#include <avr/io.h>


/****************************************
									// **
           PIN DEFINITIONS			// **
									// **
*****************************************/
									// **
#define glcd_wr_PIN		0			// **
#define glcd_wr_PORT	PORTC		// **
									// **
#define glcd_rd_PIN		1			// **
#define glcd_rd_PORT	PORTC		// **
									// **
#define glcd_ce_PIN		2			// **
#define glcd_ce_PORT	PORTC		// **
									// **
#define glcd_cd_PIN		3			// **
#define glcd_cd_PORT	PORTC		// **
									// **
#define glcd_rst_PIN	4			// **
#define glcd_rst_PORT	PORTC		// **
									// **
#define glcd_fs_PIN		5			// **
#define glcd_fs_PORT	PORTC		// **
									// **
#define glcd_rv_PIN		6			// **
#define glcd_rv_PORT	PORTF		// **
									// **
#define glcd_DATA_PORT 		PORTA	// **
#define glcd_DATA_PIN		PINA	// **
#define glcd_DATADIR		DDRA	// **
									// **
//***************************************
//***************************************

/* ----- Definitions concerning LCD internal memory  ------ */

#define glcd_FONT_SELECT  1				// Which Font? 1 = FS HIGH = narrow (6x8) / 0 = FS LOW = normal (8x8)

#if glcd_FONT_SELECT					// narrow font
    #define glcd_FONT_WIDTH 6			// pixel width
	#define glcd_BYTES_PER_ROW 40		// 40 chars per row
	#define glcd_G_BASE 0x0400			// base address of graphics memory
#else									// normal font
	#define glcd_FONT_WIDTH 8			// pixel width
	#define glcd_BYTES_PER_ROW 30		// 30 chars per row
	#define glcd_G_BASE 0x0200			// base address of graphics memory
#endif

#define glcd_T_BASE 0x0000				// base address of text memory

#define glcd_wr_high() glcd_wr_PORT  |=  _BV(glcd_wr_PIN);
#define glcd_wr_low()  glcd_wr_PORT  &= ~_BV(glcd_wr_PIN);

#define glcd_rd_high() glcd_rd_PORT  |=  _BV(glcd_rd_PIN);
#define glcd_rd_low()  glcd_rd_PORT  &= ~_BV(glcd_rd_PIN);

#define glcd_ce_high() glcd_ce_PORT  |=  _BV(glcd_ce_PIN);
#define glcd_ce_low()  glcd_ce_PORT  &= ~_BV(glcd_ce_PIN);

#define glcd_cd_high() glcd_cd_PORT  |=  _BV(glcd_cd_PIN);
#define glcd_cd_low()  glcd_cd_PORT  &= ~_BV(glcd_cd_PIN);

#define glcd_rst_high() glcd_rst_PORT  |=  _BV(glcd_rst_PIN);
#define glcd_rst_low()  glcd_rst_PORT  &= ~_BV(glcd_rst_PIN);

#define glcd_fs_high() glcd_fs_PORT  |=  _BV(glcd_fs_PIN);
#define glcd_fs_low()  glcd_fs_PORT  &= ~_BV(glcd_fs_PIN);

#define glcd_SET_DATADIR_OUT()	(glcd_DATADIR = 0xFF)
#define glcd_SET_DATADIR_IN()	(glcd_DATADIR = 0x00)

#define DDR(x) (*(&x - 1))     //address of data direction register of port x
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
   //on ATmega64/128 PINF is on port 0x00 and not 0x60
    #define PIN(x) ( &PORTF==&(x) ? _SFR_IO8(0x00) : (*(&x - 2)) )
#else
	#define PIN(x) (*(&x - 2))    //address of input register of port x
#endif


#define glcd_XMAX 240        // limits of (x,y) LCD graphics drawing
#define glcd_XMIN 0
#define glcd_YMAX 128
#define glcd_YMIN 0



/* ----------------------------------------------------------- */


void glcd_dput(int byte); 			 // write data byte to LCD module
int  glcd_dget(void);    			 // get data byte from LCD module
int  glcd_sget(void);  			 // check LCD display status pbrt
void glcd_cput(int byte);			 // write command byte to LCD module
void glcd_setup(void);			 // make sure control lines are at correct levels
void glcd_init(void);     			 // initialize LCD memory and display modes
void glcd_print(int x,int y,const char *string);  	 // send string of characters to LCD
void glcd_print_ram(int x,int y,char *string);  	 // send string of characters to LCD
void glcd_set_address(unsigned int addr);
void glcd_clear_graph(void);    		 // clear graphics memory of LCD
void glcd_clear_text(void);  		 // clear text memory of LCD
void glcd_xy(int x, int y); 		 // set memory pointer to (x,y) position (text)
void glcd_pixel(int column, int row,char show1);

void glcd_show(const char *,int,int);
void glcd_line(int x1, int y1, int x2, int y2, unsigned char show);
void glcd_circle(int x, int y, int radius, unsigned char show);
void glcd_circle_half(int x, int y, int radius, unsigned char show);
void glcd_box(int x1, int y1, int x2, int y2, unsigned char show);
void glcd_fill(int x1, int y1, int x2, int y2, unsigned char persent,char first);

void glcd_box_filled (int x1, int y1, int x2, int y2, unsigned char show);
void glcd_circle_segment(int x, int y, int radius, int winkel_start, int winkel_end, unsigned char show);
// ------------------------------------------------------------
#endif



