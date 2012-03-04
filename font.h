#ifndef __LIB_FONT_H__
#define __LIB_FONT_H__

typedef enum {
    FONT_12	= 12,
    FONT_16	= 16,
    FONT_24	= 24,
    FONT_MAX	= FONT_24,
} FONT_SIZE_T;

typedef enum {
    ASC_12,
    ASC_16,
    ASC_24,
    HZK_16,
    HZK_24,
    FONT_ERR,
} FONT_TYPE_T;

void set_lcd_row_line(FONT_TYPE_T font_type, int *rows, int *lines);
unsigned int get_bitmap(FONT_TYPE_T font_type, unsigned char *bit_buf, const unsigned char *str);
FONT_TYPE_T get_word_type(FONT_SIZE_T size, unsigned char is_hz);


#endif
