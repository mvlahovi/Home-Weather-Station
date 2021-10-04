#include <avr/pgmspace.h>
#include "font.h"
#include "t6963c.h"
#include "uart.h"

void lcd_print2_p(unsigned int x,unsigned int y, const char *in, const struct FONT_DEF *strcut1, unsigned char invers)
{
	register unsigned int offset,width;
	register unsigned char i,j,map,ertefah,allwidth=0;

	while((map = pgm_read_byte(in++))) 
	{
		map = pgm_read_byte(&strcut1->mapping_table[map]);

		width = strcut1->glyph_width;
		if(width == 0)
			width = pgm_read_byte(&strcut1->width_table[map]);

		offset = pgm_read_word(&strcut1->offset_table[map]);
		ertefah = strcut1->glyph_height;

	
		for(j=0 ; j<ertefah * (((width-1)/8)+1) ; j+=(((width-1)/8)+1)    )
		{   // ertefah
			for(i=0 ; i<width  ; i++)
			{   //  width
				if( pgm_read_byte(&strcut1->glyph_table[ offset+j+(i/8) ]) & (1 << ( 7 - ( i % 8 ) ) ) )
					glcd_pixel(  x+i+allwidth , y+j/ (((width-1)/8)+1), !invers  );
	 			else
	 				glcd_pixel(  x+i+allwidth , y+j/ (((width-1)/8)+1), invers  );
				}//End i
			}// End j
		allwidth+=width;
	}// End K
}

void lcd_print2(unsigned int x,unsigned int y, const char *in, const struct FONT_DEF *strcut1, unsigned char invers)
{
	register unsigned int offset,width;
	register unsigned char i,j,map,ertefah,allwidth=0;

	while((map = *in++)) 
	{
		map = pgm_read_byte(&strcut1->mapping_table[map]);

		width = strcut1->glyph_width;
		if(width == 0)
			width = pgm_read_byte(&strcut1->width_table[map]);

		offset = pgm_read_word(&strcut1->offset_table[map]);
		ertefah = strcut1->glyph_height;

	
		for(j=0 ; j<ertefah * (((width-1)/8)+1) ; j+=(((width-1)/8)+1)    )
		{   // ertefah
			for(i=0 ; i<width  ; i++)
			{   //  width
				if( pgm_read_byte(&strcut1->glyph_table[ offset+j+(i/8) ]) & (1 << ( 7 - ( i % 8 ) ) ) )
					glcd_pixel(  x+i+allwidth , y+j/ (((width-1)/8)+1), !invers  );
	 			else
	 				glcd_pixel(  x+i+allwidth , y+j/ (((width-1)/8)+1), invers  );
				}//End i
			}// End j
		allwidth+=width;
	}// End K
}

