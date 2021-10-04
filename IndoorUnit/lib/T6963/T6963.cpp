/*-------------------------------------------------------------------------------------------------
r10- 	see Revision.txt
-----------------------------------------------------------------------------------------------*/
#include "Arduino.h"
#include "T6963.h"
#include <pgmspace.h>
//#include "T6963_Commands.h" - Removed in R9
//-------------------------------------------------------------------------------------------------
//
// Delay function
//	
//-------------------------------------------------------------------------------------------------
void n_delay(void)
{
  volatile byte i;
  for(i = 0; i < (F_CPU/1000000); i++)
  {
    asm("nop");
  }
 // delayMicroseconds(100);
}
//-------------------------------------------------------------------------------------------------
//
// Constructor
//	
//-------------------------------------------------------------------------------------------------
T6963::T6963(int pixHoriz,int pixVert,int fontWidth, int sizeMem){
  GLCD_NUMBER_OF_LINES = pixVert;
  GLCD_PIXELS_PER_LINE = pixHoriz;
  _FW = fontWidth;
  _GA = pixHoriz / fontWidth;	//Supercedes GLCD_GRAPHIC_AREA
  _TA = pixHoriz / fontWidth;	//Supercedes GLCD_TEXT_AREA
  _sizeMem = constrain(sizeMem,0,64); //size of attached memory in kb.
  sizeGA = _GA*pixVert;		//Supercedes GLCD_GRAPHIC_SIZE
  sizeTA = _TA*pixVert/8;	        //Supercedes GLCD_TEXT_SIZE
  setTH(0);
  setGH(0);
}

void T6963::GLCD_DATA_DDR_OUT()
{pinMode (GLCD_DB0, OUTPUT);
pinMode (GLCD_DB1, OUTPUT);
pinMode (GLCD_DB2, OUTPUT);
pinMode (GLCD_DB3, OUTPUT);
pinMode (GLCD_DB4, OUTPUT);
pinMode (GLCD_DB5, OUTPUT);
pinMode (GLCD_DB6, OUTPUT);
pinMode (GLCD_DB7, OUTPUT);}

void T6963::GLCD_DATA_DDR_IN()
{pinMode (GLCD_DB0, INPUT);
pinMode (GLCD_DB1, INPUT);
pinMode (GLCD_DB2, INPUT);
pinMode (GLCD_DB3, INPUT);
pinMode (GLCD_DB4, INPUT);
pinMode (GLCD_DB5, INPUT);
pinMode (GLCD_DB6, INPUT);
pinMode (GLCD_DB7, INPUT);}



void T6963::setTH(unsigned int addr){
  _TH=addr;
}
//expose _TH
unsigned int T6963::getTH(){
  return _TH;
}
//return _TH
void T6963::setGH(unsigned int addr){
  if(addr == _TH){
    _GH=_TH+sizeTA;
  }
  else{
    _GH=addr;
  }
}
unsigned int T6963::getGH(){
  return _GH;
}
byte T6963::getTextRows(){
  return (GLCD_NUMBER_OF_LINES/8);
}
byte T6963::getTextCols(){
  return _TA;
}
byte T6963::getFw(){
  return _FW;
}
//-------------------------------------------------------------------------------------------------
//
// Ports intalization
//
//-------------------------------------------------------------------------------------------------
void T6963::InitalizeInterface(void){

  GLCD_DATA_DDR_OUT();
  pinMode (GLCD_WR, OUTPUT);pinMode (GLCD_RD, OUTPUT);pinMode (GLCD_CE, OUTPUT);pinMode (GLCD_CD, OUTPUT);
 // pinMode (GLCD_RESET,OUTPUT);
  //GLCD_CTRL_DDR = ((1 << GLCD_WR) | (1 << GLCD_RD) | (1 << GLCD_CE) | (1 << GLCD_CD));
  digitalWrite(GLCD_WR, HIGH);digitalWrite(GLCD_RD, HIGH);digitalWrite(GLCD_CE, HIGH);digitalWrite(GLCD_CD, HIGH);
 // digitalWrite(GLCD_RESET, HIGH);
  //GLCD_CTRL_PORT |= ((1 << GLCD_WR) | (1 << GLCD_RD) | (1 << GLCD_CE) | (1 << GLCD_CD));
}
//-------------------------------------------------------------------------------------------------
//
// Reads dispay status
//
//-------------------------------------------------------------------------------------------------
byte T6963::checkStatus(void){
	uint8_t tmp;

  GLCD_DATA_DDR_IN();
  digitalWrite(GLCD_RD, LOW);digitalWrite(GLCD_CE, LOW);
  n_delay();
  tmp = 0;
  if(digitalRead(GLCD_DB0)==HIGH) tmp = 1;
  if(digitalRead(GLCD_DB1)==HIGH) tmp |= 0x02;
  if(digitalRead(GLCD_DB2)==HIGH) tmp |= 0x04;
  if(digitalRead(GLCD_DB3)==HIGH) tmp |= 0x08;
  if(digitalRead(GLCD_DB4)==HIGH) tmp |= 0x10;
  if(digitalRead(GLCD_DB5)==HIGH) tmp |= 0x20;
  if(digitalRead(GLCD_DB6)==HIGH) tmp |= 0x40;
  if(digitalRead(GLCD_DB7)==HIGH) tmp |= 0x80;
 
  GLCD_DATA_DDR_OUT();
  digitalWrite(GLCD_RD, HIGH);digitalWrite(GLCD_CE, HIGH);
  return tmp;
}
//-------------------------------------------------------------------------------------------------
//
// Writes instruction 
//
//-------------------------------------------------------------------------------------------------
void T6963::writeCommand(byte command){
  while(!(checkStatus()&0x03));
  
  if (command & 0x01) digitalWrite(GLCD_DB0, HIGH);
  else digitalWrite(GLCD_DB0, LOW);
  if (command & 0x02) digitalWrite(GLCD_DB1, HIGH);
  else digitalWrite(GLCD_DB1, LOW);	 	 
  if (command & 0x04) digitalWrite(GLCD_DB2, HIGH);
  else digitalWrite(GLCD_DB2, LOW);	 
  if (command & 0x08) digitalWrite(GLCD_DB3, HIGH);
  else digitalWrite(GLCD_DB3, LOW);
  if (command & 0x10) digitalWrite(GLCD_DB4, HIGH);
  else digitalWrite(GLCD_DB4, LOW);
  if (command & 0x20) digitalWrite(GLCD_DB5, HIGH);
  else digitalWrite(GLCD_DB5, LOW);
  if (command & 0x40) digitalWrite(GLCD_DB6, HIGH);
  else digitalWrite(GLCD_DB6, LOW);
  if (command & 0x80) digitalWrite(GLCD_DB7, HIGH);
  else digitalWrite(GLCD_DB7, LOW);	 	 	 
  
  digitalWrite(GLCD_WR, LOW);digitalWrite(GLCD_CE, LOW);
  n_delay();
  digitalWrite(GLCD_WR, HIGH);digitalWrite(GLCD_CE, HIGH);
}

//-------------------------------------------------------------------------------------------------
//
// Writes data
//
//-------------------------------------------------------------------------------------------------
void T6963::writeData(byte data){
  while(!(checkStatus()&0x03));
  
  if (data & 0x01) digitalWrite(GLCD_DB0, HIGH);
  else digitalWrite(GLCD_DB0, LOW);
  if (data & 0x02) digitalWrite(GLCD_DB1, HIGH);
  else digitalWrite(GLCD_DB1, LOW);	 	 
  if (data & 0x04) digitalWrite(GLCD_DB2, HIGH);
  else digitalWrite(GLCD_DB2, LOW);	 
  if (data & 0x08) digitalWrite(GLCD_DB3, HIGH);
  else digitalWrite(GLCD_DB3, LOW);
  if (data & 0x10) digitalWrite(GLCD_DB4, HIGH);
  else digitalWrite(GLCD_DB4, LOW);
  if (data & 0x20) digitalWrite(GLCD_DB5, HIGH);
  else digitalWrite(GLCD_DB5, LOW);
  if (data & 0x40) digitalWrite(GLCD_DB6, HIGH);
  else digitalWrite(GLCD_DB6, LOW);
  if (data & 0x80) digitalWrite(GLCD_DB7, HIGH);
  else digitalWrite(GLCD_DB7, LOW);	 	 	 
  
  digitalWrite(GLCD_WR, LOW);digitalWrite(GLCD_CD, LOW);digitalWrite(GLCD_CE, LOW);
  n_delay();
  digitalWrite(GLCD_WR, HIGH);digitalWrite(GLCD_CD, HIGH);digitalWrite(GLCD_CE, HIGH);
}
//-------------------------------------------------------------------------------------------------
//
// Reads data
//
//-------------------------------------------------------------------------------------------------
byte T6963::ReadData(void){
  uint8_t tmp;
  while(!(checkStatus()&0x03));

  GLCD_DATA_DDR_IN();
  digitalWrite(GLCD_RD, LOW);digitalWrite(GLCD_CD, LOW);digitalWrite(GLCD_CE, LOW);
  n_delay();
  tmp = 0;
  if(digitalRead(GLCD_DB0)==HIGH) tmp = 1;
  if(digitalRead(GLCD_DB1)==HIGH) tmp |= 0x02;
  if(digitalRead(GLCD_DB2)==HIGH) tmp |= 0x04;
  if(digitalRead(GLCD_DB3)==HIGH) tmp |= 0x08;
  if(digitalRead(GLCD_DB4)==HIGH) tmp |= 0x10;
  if(digitalRead(GLCD_DB5)==HIGH) tmp |= 0x20;
  if(digitalRead(GLCD_DB6)==HIGH) tmp |= 0x40;
  if(digitalRead(GLCD_DB7)==HIGH) tmp |= 0x80;

  digitalWrite(GLCD_RD, HIGH);digitalWrite(GLCD_CD, HIGH);digitalWrite(GLCD_CE, HIGH);
  GLCD_DATA_DDR_OUT();
  return tmp;

}
//-------------------------------------------------------------------------------------------------
//
// Sets address pointer for display RAM memory
//
//-------------------------------------------------------------------------------------------------
void T6963::SetAddressPointer(unsigned int address){
  writeData(address & 0xFF);
  writeData(address >> 8);
  writeCommand(T6963_SET_ADDRESS_POINTER);
}
//-------------------------------------------------------------------------------------------------
//
// Clears text area of display RAM memory
//
//-------------------------------------------------------------------------------------------------
void T6963::clearText(){
  SetAddressPointer(_TH);
  for(int i = 0; i < sizeTA; i++){
    WriteDisplayData(0);
  }
}
//-------------------------------------------------------------------------------------------------
// Clears characters generator area of display RAM memory
//-------------------------------------------------------------------------------------------------
void T6963::clearCG(){
  unsigned int i=((_sizeMem/2)-1)*0x800;
  SetAddressPointer(i);
  for(i = 0; i < 256 * 8; i++){
    WriteDisplayData(0);
  }
}
//-------------------------------------------------------------------------------------------------
// Clears graphics area of display RAM memory
//-------------------------------------------------------------------------------------------------
void T6963::clearGraphic(){
  SetAddressPointer(_GH);
  for(unsigned int i = 0; i < sizeGA; i++){
    WriteDisplayData(0x00);
  }
}
//-------------------------------------------------------------------------------------------------
// Writes a single character (ASCII code) to display RAM memory
//-------------------------------------------------------------------------------------------------
void T6963::writeChar(char charCode){
  WriteDisplayData(charCode - 32);
  if (++cursor_addr>640) cursor_addr=0;
}
void T6963::writeCharDec(char charCode){
  writeDataDec(charCode - 32);
  if (++cursor_addr>640) cursor_addr=0;
}
//-------------------------------------------------------------------------------------------------
// Writes null-terminated string to display RAM memory
//
// Example: LCD.TextGoTo(0,3); //Position
//			LCD.writeString("Hello World"); Print String "Hello World"
//-------------------------------------------------------------------------------------------------
void T6963::writeString(const char * string){
  while(*string){
    if (*string == '\n')
    {
        cursor_addr+=40;
        TextGoTo(cursor_addr%40,cursor_addr/40);
    }
    else 
    {
        if (*string == '\r') 
        {
          cursor_addr = (cursor_addr/40) * 40;
          TextGoTo(cursor_addr%40,cursor_addr/40);
        }
        else writeChar(*string);
    }
   string++;   
  }
}
//-------------------------------------------------------------------------------------------------
// Print String "Hello World" with Font (byte) faster
//
// Example: LCD.TextGoTo(0,0); //Position
//	           LCD.glcd_print2_P(0,0, "Hello World", &Segoe_Script__14,1);
//-------------------------------------------------------------------------------------------------
void T6963::glcd_print2_P(unsigned char x,unsigned char y, const char *in, const struct FONT_DEF *strcut1, unsigned char invers)
{
	unsigned int offset,swbyte=0;
	unsigned char width, by=0, sbyte=0, tmp=0;
	unsigned char NrBytes;
	unsigned char i,j,map,height,Fontwidth=0;

	while((map = *in++)) 
	{
		map = pgm_read_byte(&strcut1->mapping_table[map]);

		width = strcut1->glyph_width;
		if(width == 0)
			width = pgm_read_byte(&strcut1->width_table[map]);
                
                NrBytes = ((width-1)/8)+1;
                
		offset = pgm_read_word(&strcut1->offset_table[map]);
		height = strcut1->glyph_height;
       
       		for(j=0; j<height*NrBytes; j+=NrBytes)      // height
				{   
                   swbyte=0;
                   GraphicGoTo(x+Fontwidth,y+j/NrBytes); //Graphics Pointer
                   
				for(i=0; i<NrBytes; i++)            //  width
                    {   
                    by = pgm_read_byte(&strcut1->glyph_table[offset+j+i]); //Read 8bit
                        if( _FW == 6)  //fontWidth
                        {
                             switch(swbyte)         //convert 8bit to 6bit Data for 6bit Font
                            {
							 case 0:                //First Byte
                             sbyte=by>>2;           //First 6 Bit out
                             tmp=by<<4;             //Store last 2 Bits from first Byte
                             swbyte++;
                             break;
                             case 1:               //First Byte and second Byte
                             sbyte=tmp+(by>>4);    //Second 6 Bit out
                             tmp=by<<2;            //Store last 4 Bits from second Byte
                             swbyte++;
                             break;
                             case 2:               //Second and third Byte
                             sbyte=tmp+(by>>6);    //Third 6 Bit out
                             tmp=by;               //Store last 6 Bits from third Byte
                             swbyte++;
                             break;
                             case 3:               //last 6 bits from the third byte
                             sbyte=tmp;            //Fourth 6 Bit out
                             i-=1;                 //i Counter correction while read by
                             swbyte=0; 
                             break;
                            }                    //End convert 8bit to 6bit Font
                        WriteDisplayData(sbyte); //6bit Font
                        }
                        else
                          WriteDisplayData(by);  // 8bit Font
					}//End i
                }// End j
		Fontwidth+=_FW*NrBytes;               
	}// End K
   
}
//-------------------------------------------------------------------------------------------------
// Print String "Hello World" with Font (Pixel) slower
//
// Example: LCD.TextGoTo(0,0); //Position
//		LCD.glcd_print1_P(0,0, "Hello World", &Segoe_Script__14,1);
//-------------------------------------------------------------------------------------------------
void T6963::glcd_print1_P(unsigned char x,unsigned char y, const char *in, const struct FONT_DEF *strcut1, unsigned char invers)
{
	unsigned int offset;
	unsigned char width, by=0, mask=0;
	unsigned char NrBytes;
	unsigned char i,j,map,height,allwidth=0;

	while((map = *in++)) 
	{
		map = pgm_read_byte(&strcut1->mapping_table[map]);

		width = strcut1->glyph_width;
		if(width == 0)
			width = pgm_read_byte(&strcut1->width_table[map]);

		offset = pgm_read_word(&strcut1->offset_table[map]);
		height = strcut1->glyph_height;

        NrBytes = ((width-1)/8)+1;
	
		for(j=0; j<height * NrBytes; j+=NrBytes    )
		{   // height
			for(i=0 ; i<width  ; i++)
			{   //  width
			    if(i%8 == 0)
			    {
			     by = pgm_read_byte(&strcut1->glyph_table[ offset+j+(i/8) ]);
			     mask = 0x80;
			    }

				if( by & mask )
					writePixel(  x+i+allwidth , y+j/ NrBytes, !invers  );
	 			else
	 				writePixel(  x+i+allwidth , y+j/ NrBytes, invers  );

	 			mask >>= 1;
			}//End i
		}// End j
		allwidth+=width;
	}// End K
} 

//-------------------------------------------------------------------------------------------------
// Writes null-terminated string from program memory to display RAM memory
//-------------------------------------------------------------------------------------------------
void T6963::writeStringPgm(const __FlashStringHelper * str){
  if (!str) return; // return if the pointer is void
    int lenght = strlen_P((PGM_P)str); // cast it to PGM_P, which is basically const char *, and measure it using the _P version of strlen.
    if (lenght == 0) return;
    char buffer[lenght+2];
    strcpy_P(buffer, (PGM_P)str); //copy the string in using strcpy_P

  char ch;
  char * buff;
  buff=buffer;
  while((ch = *buff))
  {
    if (ch == '\n')
    {
        cursor_addr+=40;
        TextGoTo(cursor_addr%40,cursor_addr/40);
    }
    else 
    {
        if (ch == '\r') 
        {
          cursor_addr = (cursor_addr/40) * 40;
          TextGoTo(cursor_addr%40,cursor_addr/40);
        }
        else writeChar(ch);
    }
    buff++;   
  }
}
//-------------------------------------------------------------------------------------------------
// Sets display coordinates
//-------------------------------------------------------------------------------------------------
void T6963::TextGoTo(unsigned char x, unsigned char y){
  unsigned int address;
  address = _TH +  x + (_TA * y);
  SetAddressPointer(address);
  cursor_addr = address;
}
//-------------------------------------------------------------------------------------------------
// Writes single char pattern to character generator area of display RAM memory
//-------------------------------------------------------------------------------------------------
/*void T6963::DefineCharacter(byte charCode, unsigned char * defChar){
//#define GLCD_EXTERNAL_CG_HOME		(2 << 11) ili 0x1400
  unsigned int address=((_sizeMem/2)-1)*0x800;	//sizeMem =64
  SetAddressPointer(address);
  for(byte i = 0; i < 8 ; i++){
    WriteDisplayData(*(defChar + i));
  }
}
*/
void T6963::DefineCharacter(byte charCode, const unsigned char * defChar){

  unsigned int address=0xE400;	//pogledati HITACHI app note

    SetAddressPointer(address+(unsigned int)charCode*8);
  for(byte i = 0; i < 8 ; i++){
    WriteDisplayData(*(defChar + i));
  }
}

void T6963::writeExtChar(char charCode){
  WriteDisplayData(0x80 + charCode);
}

//-------------------------------------------------------------------------------------------------
// Set (if color==1) or clear (if color==0) pixel on screen  dodao 2 xor
//-------------------------------------------------------------------------------------------------
void T6963::writePixel(byte x, byte y, byte color){
  unsigned char tmp;
  unsigned int address;
  address = _GH + (x / _FW) + (_GA * y);
  SetAddressPointer(address);
  writeCommand(T6963_DATA_READ_AND_NONVARIABLE);
  tmp = ReadData();
  
  switch(color){
	case 2: tmp ^= (1 <<  (_FW - 1 - (x % _FW)));
			break;	
    case 1: tmp |= (1 <<  (_FW - 1 - (x % _FW)));
			break;
    case 0: tmp &= ~(1 <<  (_FW - 1 - (x % _FW)));
			break;
	default: tmp |= (1 <<  (_FW - 1 - (x % _FW)));
  }
  
  WriteDisplayData(tmp);
}
//-------------------------------------------------------------------------------------------------
// Set a single pixel at x,y (in pixels) to 1 (on)
//-------------------------------------------------------------------------------------------------
byte T6963::setPixel(byte x, byte y){
  SetAddressPointer((_GH + (x / _FW) + (_GA * y)));
  byte tmp=B11111000;
  tmp |= (_FW-1)-(x%_FW); //LSB Direction Correction
  writeCommand(tmp);
  return tmp;
}
//-------------------------------------------------------------------------------------------------
// Set a single pixel at x,y (in pixels) to 0 (off)
//-------------------------------------------------------------------------------------------------
byte T6963::clearPixel(byte x, byte y){
  SetAddressPointer((_GH + (x / _FW) + (_GA * y)));
  byte tmp=B11110000;
  tmp |= (_FW-1)-(x%_FW); //LSB Direction Correction
  writeCommand(tmp);
  return tmp;
} 
//-------------------------------------------------------------------------------------------------
// Writes display data and increment address pointer
//-------------------------------------------------------------------------------------------------
void T6963::WriteDisplayData(byte x){
	writeData(x);
	writeCommand(T6963_DATA_WRITE_AND_INCREMENT);
}
void T6963::writeDataNon(byte x){
	writeData(x);
	writeCommand(T6963_DATA_WRITE_AND_NONVARIALBE);
}
void T6963::writeDataDec(byte x){
	writeData(x);
	writeCommand(T6963_DATA_WRITE_AND_DECREMENT);
}
//-------------------------------------------------------------------------------------------------
// Sets graphics coordinates
//-------------------------------------------------------------------------------------------------
void T6963::GraphicGoTo(byte x, byte y){
  unsigned int address;
  address = _GH + (x / _FW) + (_GA * y);
  SetAddressPointer(address);
}
//-------------------------------------------------------------------------------------------------
// Displays bitmap from program memory
//-------------------------------------------------------------------------------------------------
/*
void T6963::Bitmap(unsigned char * bitmap, byte x, byte y, byte width, unsigned char height){
 unsigned char i, j;
 for(j = 0; j < height; j++){
 GraphicGoTo(x, y + j);
 for(i = 0; i < width/_FW; i++){
 WriteDisplayData(pgm_read_byte(bitmap + i + (_GA * j)));
 }
 }
 }
 */
 
//-----------------------------------------------------------------------
//               Set Mode for Step 1 Text Manipulation and Show CGRam
//Example: LCD.setMode('0','I');           
//         
//Parameter: '0=Normal' 'X=Xor' 'A=And' 'T=Text Attribute Mode'
//			 , 'I=Intern CGram' , 'E=Extern CGram'
// Show Step 2 Example: (Set Text Attribute) to complete
//-----------------------------------------------------------------------
byte T6963::setMode(char _mode, char _CG){
  byte tmp = T6963_MODE_SET;
  if(_mode=='X' || _mode =='x' || _mode=='^'){
    tmp |= 1;
  }
  else if(_mode == 'A' || _mode=='&' ||_mode=='a'){
    tmp |= 3;
  }
  else if(_mode == 'T'||_mode=='t'){
    tmp |=4;
  }
  else{
    tmp |= 0; //OR mode default
  }
  if(_CG =='E'||_CG=='e'){
    tmp |=8;
  }
  else{
    tmp |=0;
  }
  writeCommand(tmp);
  return tmp;
}

//-----------------------------------------------------------------------
//                               Set Text Attribute (Text only) (Step 2)
//Example: LCD.setMode('T','I');         
//         LCD.setTextAttrMode('0');     
//
//Parameter: '0=Normal display' '5=Reverse display' '3=Inhibit display' 
//			 '8=Blink of normal display' 'D=Blink of reverse display' 
//			 'B=Blink of inhibit display'
//-----------------------------------------------------------------------
byte T6963::setTextAttrMode(char _mode){   // Text only: 0=Normal display 5=Reverse display 3=Inhibit display 8=Blink of normal display D=Blink of reverse display B=Blink of inhibit display
  
  byte tmp = 0;
  if(_mode=='3'){
    tmp |= 0b0011;
  }
  else if(_mode == '5'){
    tmp |= 0b0101;
  }
  else if(_mode == '8'){
    tmp |=0b1000;
  }
  else if(_mode == 'D'||_mode=='d'){
    tmp |=0b1101;
  }
  else if(_mode == 'B'||_mode=='b'){
    tmp |=0b1011;
  }
  else{
    tmp |= 0; //Normal Text mode default
  }
   SetAddressPointer(_GH);
  for(unsigned int i = 0; i < sizeGA; i++){
    WriteDisplayData(tmp);    
  }
  return tmp;
}
//-----------------------------------------------------------------------

void T6963::clearDispMode(){
  writeCommand(T6963_DISPLAY_MODE);
}

//-----------------------------------------------------------------------
//                       Set Display Mode   
//Example: LCD.setDispMode(true,true,false,false);    //true=ON	false=off 
//
//Parameter: ('GRAPHIC', 'TEXT', 'CURSOR', 'CURSOR BLINK')
//			  
//Set blinking Cursor Example: 	LCD.setDispMode(true,true,true,true); //Mode all ON
//								LCD.setCursorPattern(8); // Cursor high
//								LCD.setCursorPointer(0,0); //Cursor Position	 
//-----------------------------------------------------------------------
byte T6963::setDispMode(boolean _text,boolean _graphics, boolean _cursor, boolean _blink)
{
  byte tmp=T6963_DISPLAY_MODE;
  if(_graphics){
    tmp |= B1000; //T6963_GRAPHIC_DISPLAY_ON
  }
  if(_text){
    tmp |= B0100; //T6963_TEXT_DISPLAY_ON
  }
  if(_cursor){
    tmp |= B0010; //T6963_CURSOR_DISPLAY_ON
  }
  if(_blink){
    tmp |=B0001; //T6963_CURSOR_BLINK_ON
  }
  writeCommand(tmp);
  return tmp;
}

//-----------------------------------------------------------------------
//                               Show Picture(beta)
//Generate Picture File  by "BMP2C.exe datei.bmp 6" example for 6*8 Pixel
//http://www.holger-klabunde.de
//6 pixel per byte for T6963 and 6x8 font
//Horizontal pixel orientation for T6963
//Pixel orientation per Byte: D7..D0
// 
//Example: #include <demo.h>									//Filename
//		   LCD.drawPic( 0, 0, demobmp ,demoHEIGHT, demoBYTEWIDTH );; //Show Demo Picture

// Parameter :  x,y = Position, Picturename (not Filename see in Picturefile), Height in Pixel, Widht in Byte
//-----------------------------------------------------------------------
 void T6963::drawPic(unsigned int x, unsigned int y, const char * PicName, unsigned int HEIGHT, unsigned int BYTEWIDTH) //HEIGHT = max 64;	//Height in pixels     BYTEWIDTH = max 40;	//Width in bytes by 6x8 Font
{ 
	byte bitmap;
	int j;
	
	j=0;
               
  	for(int h = 0; h < HEIGHT; h++)
	{
		GraphicGoTo(x,y+h);
		
	
         for(int i = 0; i < BYTEWIDTH; i++)     //(PIXELS PER LINE/FONT WIDTH * NUMBER OF LINES)
         {
           bitmap = pgm_read_byte(PicName+(i+j));
           WriteDisplayData(bitmap);
         }
	 j=j+BYTEWIDTH;
    }
}
   
  

/*------------------------------------------------------------------------
Geometric shapes, scavenged from online resources.
why re-invent the wheel
*/

//-------------------------------------------------------------------------------------------------
//                               Show Rectangel (WritePixel)
////Example: LCD.createLine (0, 0, 30, 20);
//
//Parameter: createLine(int x0,int y0,int x1,int y1) x0 and y0 left Position  x1 and y1 right Position
//-------------------------------------------------------------------------------------------------
void T6963::createLine(int x0,int y0,int x1,int y1, byte color)
{
/*BreshenhamLine algorithm - From wikipedia so it must be right
http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
*/
  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if(steep){
    //swap(x0, y0)
    //swap(x1, y1)
    int tmp=x0;
    x0=y0;
    y0=tmp;
    tmp=x1;
    x1=y1;
    y1=tmp;
  }
  if(x0 > x1){
    //swap(x0, x1)
    //swap(y0, y1)
    int tmp=x0;
    x0=x1;
    x1=tmp;
    tmp=y0;
    y0=y1;
    y1=tmp;
  }
  int deltax = x1 - x0;
  int deltay = abs(y1 - y0);
  int error = deltax / 2;
  int ystep=-1;
  int y = y0;
  if(y0 < y1){ystep= 1;}
  for(int x =x0;x<=x1;x++){
    if(steep){writePixel(y,x,color);}else{writePixel(x,y,color);}
    error = error - deltay;
    if(error < 0){
      y = y + ystep;
      error = error + deltax;
    }
  }
}
/*------------------------------------------------------------------------------------------------
Circles: http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
------------------------------------------------------------------------------------------------*/

//-------------------------------------------------------------------------------------------------
//                               Show Rectangel (writePixel)
////Example: LCD.Rectangle (0, 0, 30, 20, 1);
//
//Parameter: Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a, color)    x1 and y1 top left Position b and a Size of the rectangle
//-------------------------------------------------------------------------------------------------


void T6963::Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a, byte color)
{
  unsigned char j; 
  // Draw vertical lines
  for (j = 0; j < a; j++) {
		writePixel(x, y + j, color);
		writePixel(x + b - 1, y + j, color);
	}
  // Draw horizontal lines
  for (j = 0; j < b; j++)	{
		writePixel(x + j, y, color);
		writePixel(x + j, y + a - 1, color);
	}
}

//-------------------------------------------------------------------------------------------------
//                               Show filled Box (WritePixel)
////Example: LCD.fillRect (0,0,50,10,1);
//
//Parameter: fillRect (int x1, int y1, int x2, int y2, color) x1 and y1 top left Position  x2 and y2 bottom right Position
//-------------------------------------------------------------------------------------------------
void T6963::fillRect(int x1, int y1, int x2, int y2, byte color)
{
	for (int i = y1; i <= y2; i++)
		 createLine(x1, i, x2, i, color);
}

//-------------------------------------------------------------------------------------------------
//                               Show filled Box (BYTE)
////Example: LCD.setDispMode(true,true,false,false);    //true=ON	false=off 
//
//Parameter: ('GRAPHIC', 'TEXT', 'CURSOR', 'CURSOR BLINK')
//			  
// drawrectbyte(x, y, height, bytewidth, pattern) // x,y = Start Position, Height in Pixel, Widht in Byte, pattern in byte(0b11111111)
//-------------------------------------------------------------------------------------------------

void T6963::drawrectbyte(unsigned int x, unsigned int y, unsigned int HEIGHT, unsigned int BYTEWIDTH, byte pattern)    //HEIGHT = max 64;	//Height in pixels     BYTEWIDTH = max 40;	//Width in bytes by 6x8 Font
{ 
  int j;
	
  GraphicGoTo(x,y);
  j=0;
    
  for(int h = 0; h < HEIGHT; h++)
  {
    GraphicGoTo(x,y+h);
    	
         for(unsigned int i = 0; i < BYTEWIDTH; i++)     //(PIXELS PER LINE/FONT WIDTH * NUMBER OF LINES)
         {
           WriteDisplayData(pattern);
         }
	j=j+BYTEWIDTH;
       }
     } 
//-------------------------------------------------------------------------------------------------
//                               Show Circle
////Example: LCD.createCircle(30, 30, 30, 1);
//
//Parameter: createCircle(int cx, int cy, int radius, color) //cx and cy mark the distance from the origin point.
//			  
//-------------------------------------------------------------------------------------------------
	
void T6963::createCircle(int cx, int cy, int radius, byte color)
{
	int error = -radius;
	int x = radius;
	int y = 0;
 
	while (x >= y){
		plot8points(cx, cy, x, y, color);
 
		error += y;
		++y;
		error += y;
 
		if (error >= 0){
			--x;
			error -= x;
			error -= x;
		}
	}
}
 
void T6963::plot8points(int cx, int cy, int x, int y, byte color)
{
	plot4points(cx, cy, x, y, color);
	if (x != y) plot4points(cx, cy, y, x, color);
}
 
void T6963::plot4points(int cx, int cy, int x, int y, byte color)
{
	writePixel(cx + x, cy + y, color);
	if (x != 0) writePixel(cx - x, cy + y, color);
	if (y != 0) writePixel(cx + x, cy - y, color);
	if (x != 0 && y != 0) writePixel(cx - x, cy - y, color);
}



//-------------------------------------------------------------------------------------------------
//
// Display initalization
//
//-------------------------------------------------------------------------------------------------
void T6963::Initialize(void)
{
  //Set up data and control ports
  InitalizeInterface();

  //reset LCD, should be able to have hardware solution
  //GLCD_CTRL_PORT &= ~(1 << GLCD_RESET);
  //_delay_ms(1);
  //GLCD_CTRL_PORT |= (1 << GLCD_RESET);
  //digitalWrite(GLCD_RESET, LOW);
  //delay(2);
  //digitalWrite(GLCD_RESET, HIGH);
  //Set Graphics Home Address
  writeData(_GH & 0xFF);
  writeData(_GH >> 8);
  writeCommand(T6963_SET_GRAPHIC_HOME_ADDRESS);

  //Set Graphics Area
  writeData(_GA);
  writeData(0x00);
  writeCommand(T6963_SET_GRAPHIC_AREA);

  //Set Text home address
  writeData(_TH & 0xFF);
  writeData(_TH >> 8);
  writeCommand(T6963_SET_TEXT_HOME_ADDRESS);

  //Set Text Area
  writeData(_TA);
  writeData(0x00);
  writeCommand(T6963_SET_TEXT_AREA);

  //Set Internal CGRAM address
  writeData(0x1C); //pogledati HITACHI app note
  //writeData(((_sizeMem/2)-1));
  writeData(0x00);
  writeCommand(T6963_SET_OFFSET_REGISTER);


  setDispMode(true,true,false,false);

  setMode('O','I');
  //clearCG();
  clearText();
  clearGraphic();
  delay(100);

}
//-------------------------------------------------------------------------------------------------
//
// Cursor Controls
//
//-------------------------------------------------------------------------------------------------
byte T6963::setCursorPattern(byte _b)
{
  byte tmp = T6963_CURSOR_PATTERN_SELECT;
  _b=constrain(_b,0,7);
  tmp|=_b;
  writeCommand(tmp);
  return tmp;
}

void T6963::setCursorPointer(byte _col,byte _row)
{
  _col=constrain(_col,0,(_TA-1));
  _row=constrain(_row,0,((GLCD_NUMBER_OF_LINES/8)-1));
  writeData(_col);
  writeData(_row);
  writeCommand(0x21); //Cursor pointer Set
}
