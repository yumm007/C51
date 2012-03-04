#include <stdio.h>
#include <string.h>
#include "font.h"
#include "lib.h"
#include "spi.h"
#include "lcd.h"

//typedef enum {
#define    ASC_12_OFFS  0
#define    ASC_16_OFFS	 (ASC_12_OFFS + 1536)
#define    ASC_24_OFFS	 (ASC_16_OFFS + 4096)
#define    HZK_16_OFFS	 (ASC_24_OFFS + 12288)
#define    HZK_24_OFFS	 (HZK_16_OFFS + 267616)
//} FONT_OFFSET_T;

#define LCD_ROW	240
#define LCD_LINE	320
#define LCD_LINE_EMPTY	0	//字符之间隔一个像素

//返回字体类型
FONT_TYPE_T get_word_type(FONT_SIZE_T size, unsigned char is_hz) {
    FONT_TYPE_T ret;

    switch (size){
	case FONT_12:
	    ret = !is_hz ? ASC_12: FONT_ERR;
	    break;
	case FONT_16:
	    ret = !is_hz ? ASC_16: HZK_16;
	    break;
	case FONT_24:
	    ret = !is_hz ? ASC_24: HZK_24;
	    break;
	default:
	    ret = FONT_ERR;
	    break;
    }

    return ret;
}

unsigned int get_bitmap(FONT_TYPE_T font_type, unsigned char *bit_buf, const unsigned char *str) {
    float offset;
	u16 offset_h = 0, offset_l = 0; 
	int len = 0;

    switch (font_type) {
	case ASC_12:
	    len = 12 * 1;	//12的字库，实际上是12 *8 bit的
	    offset = ASC_12_OFFS + (*str) * len;
		offset_h = 0;
		offset_l = offset;			
	    break;
	case ASC_16:
	    len = 16 * 1;	//16 * 8
	    offset = ASC_16_OFFS + (*str) * len;
		offset_h = 0;
		offset_l = offset;			
	    break;
	case ASC_24:
	    len =  24 * 2; //24 * 16
	    offset = ASC_24_OFFS + (*str) * len;
		offset_h = 0;
		offset_l = offset;			
	    break;
	case HZK_16:
	    len = 16 * 2;
	    offset = HZK_16_OFFS + (float)(94*(str[0] - 0xa0 -  1) + (float)(str[1] - 0xa0 -1)) * len;
		offset_h = offset / 65536;
		offset_l = (u16)offset;			
	    break;
	case HZK_24:
	    len = 24 * 3;
	    offset = HZK_16_OFFS + 267616.0 + (float)(94*(str[0] - 0xa0  - 15 - 1) + (float)(str[1] - 0xa0 -1)) * len;
		offset_h = offset / 65536;
		offset_l = (u16)offset;			
	    break;
	default:
	    break;	
    }

    spi_read(offset_h, offset_l, bit_buf, len);

    return len;
}

//从低位到高位，位为0表示描背景色，位为1表示为字体颜色；
static void send_8bit(FONT_TYPE_T font_type, unsigned char tmp) {
    int i;
    static int j = 0;
    int flag = 0;

    for (i = 7; i >= 0; i--)
		printf("%s", tmp & (1 << i) ? "--" : "  ");

    j += 8;
    switch (font_type) {
	case ASC_12:
	    flag = j == 8;
	    break;
	case ASC_16:
	    flag = j == 8;
	    break;
	case ASC_24:
	    flag = j == 16;
	    break;
	case HZK_16:
	    flag = j == 16;
	    break;
	case HZK_24:
	    flag = j == 24;
	    break;
	default:
	    break;
    }

    if (flag) {
		printf("\n");
		j = 0;
    }

}


//设置屏幕起始和结束为止,并将
//row, line指向下一个空白位置,可能换行也可能跳到行首
void set_lcd_row_line(FONT_TYPE_T font_type, int *rows, int *lines) {
    extern void MainLCD_Window_Set(unsigned short sax, unsigned short say, unsigned short eax, unsigned short eay);
	
	int font_size_r, font_size_l, cur_row = *rows, cur_line = *lines, next_row, next_line;

	switch (font_type) {
	case ASC_12:
	    font_size_r = 8;
	    font_size_l = 12;
	    break;
	case ASC_16:
	    font_size_r = 8;
	    font_size_l = 16;
	    break;
	case ASC_24:
	    font_size_r = 16;
	    font_size_l = 24;
	    break;
	case HZK_16:
	    font_size_r = 16;
	    font_size_l = 16;
	    break;
	case HZK_24:
	    font_size_r = 24;
	    font_size_l = 24;
	    break;
	default:
	    font_size_r = 0;
	    font_size_l = 0;
	    break;
    }

	if (cur_row + font_size_r > LCD_ROW) {
		cur_row = 0;
		cur_line += font_size_l + LCD_LINE_EMPTY;
	}

	if (cur_line + font_size_l > LCD_LINE) {
		cur_line = 0;
		cur_row = 0;
	}

	next_row = 	 cur_row + font_size_r;
	next_line =  cur_line + font_size_l;

	MainLCD_Window_Set(cur_row, cur_line, next_row-1, next_line-1);


	next_row += LCD_LINE_EMPTY;
	next_line += LCD_LINE_EMPTY;

    *rows = next_row;
	*lines = cur_line;

}
