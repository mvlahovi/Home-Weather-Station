#ifndef __FONT__
#define __FONT__

struct FONT_DEF {
   const unsigned int glyph_width;                 /* glyph width in pixels              */
                                             /*     0 for variable width fonts     */
   const unsigned int glyph_height;                /* glyph height for storage           */
   const unsigned char *glyph_table;         /* font table start address in memory */
   const unsigned char *mapping_table;       /* used to find the index of a glyph  */
   const unsigned char *width_table;         /* width table start adress           */
                                             /*        NULL for fixed width fonts  */
   const unsigned int *offset_table;         /* ks the offsets of the first byte*/
                                             /*                      of each glyph */
} ;


extern unsigned char pow2(unsigned char b);            
extern void lcd_print2_p(unsigned int x,unsigned int y, const char *in, const struct FONT_DEF *strcut1, unsigned char invers);
extern void lcd_print2(unsigned int x,unsigned int y, const char *in, const struct FONT_DEF *strcut1, unsigned char invers);

#define lcd_print2_P(__x, __y, __in, __s1, __i)       lcd_print2_p(__x,__y,PSTR(__in),__s1,__i)

#endif
                                  

