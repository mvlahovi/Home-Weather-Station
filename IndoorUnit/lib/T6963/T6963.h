/*-------------------------------------------------------------------------------------------------
r10-	see Revision.txt
-----------------------------------------------------------------------------------------------*/
#ifndef T6963_h
#define T6963_h

#include "Arduino.h"
#include "inttypes.h"
//#include "avr/io.h"
#include "pgmspace.h"
//#include "util/delay.h"
#include "font.h"


// control signals
#define GLCD_DB0		13
#define GLCD_DB1		12
#define GLCD_DB2		14
#define GLCD_DB3		27
#define GLCD_DB4		26
#define GLCD_DB5		25
#define GLCD_DB6		33
#define GLCD_DB7		32

#define GLCD_WR 		4
#define GLCD_RD			15
#define GLCD_CE			16  //Should be able to XNOR this with WR and RD
#define GLCD_CD			17
//#define GLCD_RESET  2  //For some reason my LCD works with this pin resistored to +5
//#define GLCD_FS     		5  //Use hardware solution not pin.
//#define GLCD_BL 		3


// display properties

const byte T6963_CURSOR_PATTERN_SELECT=0xA0; //cursor patter select command prefix or with desired size-1.
const byte T6963_DISPLAY_MODE=0x90; 
const byte T6963_MODE_SET=0x80;
const byte T6963_SET_CURSOR_POINTER=0x21;
const byte T6963_SET_OFFSET_REGISTER=0x22;
const byte T6963_SET_ADDRESS_POINTER=0x24;
const byte T6963_SET_TEXT_HOME_ADDRESS=0x40;
const byte T6963_SET_TEXT_AREA=0x41;
const byte T6963_SET_GRAPHIC_HOME_ADDRESS=0x42;
const byte T6963_SET_GRAPHIC_AREA=0x43;
const byte T6963_SET_DATA_AUTO_WRITE=0xB0;
const byte T6963_SET_DATA_AUTO_READ=0xB1;
const byte T6963_AUTO_RESET=0xB2;

const byte T6963_DATA_WRITE_AND_INCREMENT=0xC0;
const byte T6963_DATA_READ_AND_INCREMENT=0xC1;
const byte T6963_DATA_WRITE_AND_DECREMENT=0xC2;
const byte T6963_DATA_READ_AND_DECREMENT=0xC3;
const byte T6963_DATA_WRITE_AND_NONVARIALBE=0xC4;
const byte T6963_DATA_READ_AND_NONVARIABLE=0xC5;

const byte T6963_SCREEN_PEEK=0xE0;
const byte T6963_SCREEN_COPY=0xE8;

class T6963{
public:
  T6963(int pixHoriz,int pixVert,int fontWidth, int sizeMem); //Class
  //pixHoriz = Horizontal resolution
  //pixVert = Vertical Resolution
  //fontWidth = pixel width of font, determines number of columns, use hardware setting for LCD, no outputs linked to this variable.
  //sizeMem = size of memory attached to LCD in kb s/b 4,8,16,32,64 type number.
  void GLCD_DATA_DDR_OUT(void);
  void GLCD_DATA_DDR_IN(void);
  void InitalizeInterface(void);
  void writeCommand(byte);
  void writeData(byte);
  byte ReadData(void);
  void clearText(void);
  void clearCG(void);
  void clearGraphic(void);
  void writeChar(char ch);
  void writeCharDec(char charCode);
  void writeString(const char * str);
  void glcd_print2_P(unsigned char, unsigned char, const char*, const FONT_DEF*, unsigned char);
  void glcd_print1_P(unsigned char, unsigned char, const char*, const FONT_DEF*, unsigned char);
  void writeStringPgm(const __FlashStringHelper * str);
  void TextGoTo(byte, byte);
  void GraphicGoTo(byte x, byte y);
  void DefineCharacter(unsigned char, const unsigned char *);
  void writeExtChar(char charCode);
  void Initialize(void);
  void writePixel(byte, byte, byte);
  byte clearPixel(byte,byte);
  byte setPixel(byte,byte);
  void WriteDisplayData(byte);
  byte checkStatus(void);
  void SetAddressPointer(unsigned int address);
  byte setMode(char, char);
  void clearDispMode();
  byte setDispMode(boolean _text,boolean _graphics, boolean _cursor, boolean _blink);
  byte setTextAttrMode(char);
  
  byte setCursorPattern(byte _b);
  void setCursorPointer(byte _col,byte _row);

  void writeDataNon(byte); //Intended to replace WriteDisplayDatat()
  void writeDataDec(byte);

  void drawPic(unsigned int, unsigned int, const char *, unsigned int, unsigned int);
  void createLine( int, int, int, int, byte);
  void Rectangle(unsigned char, unsigned char, unsigned char, unsigned char, byte);
  void fillRect(int, int, int, int, byte);
  void drawrectbyte(unsigned int, unsigned int, unsigned int, unsigned int, byte);
  void createCircle(int, int, int, byte);
  
  void setTH(unsigned int addr);
  unsigned int getTH();
  void setGH(unsigned int addr);
  unsigned int getGH();
  unsigned int sizeGA;	//(GLCD_GRAPHIC_AREA * GLCD_NUMBER_OF_LINES)
  unsigned int sizeTA;	//(GLCD_TEXT_AREA * (GLCD_NUMBER_OF_LINES/8))
  byte getTextRows();
  byte getTextCols();
  byte getFw(); //Fontwidth
   
private:
  void plot8points(int, int, int, int, byte);
  void plot4points(int, int, int, int, byte);
  unsigned int GLCD_NUMBER_OF_LINES;	//Veritical Resolution, Lines
  unsigned int GLCD_PIXELS_PER_LINE;	//Horizontal Resolution, pixels per line
  unsigned int _FW;		//Font Width
  unsigned int _GH;		//Graphics home
  unsigned int _TH;		//Text Home
  unsigned int _GA;		//(GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
  unsigned int _TA;		//(GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
  byte _sizeMem;  //memory location for CGRAM (2kb required, 64kb max memory controlled by T6963, 32 possible locations)
  unsigned int cursor_addr;
};


#endif
