#include <stdio.h>
#include <string.h>

typedef enum {
    FONT_12	= 12,
    FONT_16	= 16,
    FONT_24	= 24,
    FONT_MAX	= FONT_24,
} FONT_SIZE_T;

typedef enum {
    ASC_12_OFFS = 0,
    ASC_16_OFFS	= ASC_12_OFFS + 1536,
    ASC_24_OFFS	= ASC_16_OFFS + 4096,
    HZK_16_OFFS	= ASC_24_OFFS + 12288,
    HZK_24_OFFS	= HZK_16_OFFS + 267616,
} FONT_OFFSET_T;

typedef enum {
    ASC_12,
    ASC_16,
    ASC_24,
    HZK_16,
    HZK_24,
    FONT_ERR,
} FONT_TYPE_T;

#define LCD_ROW	240
#define LCD_LINE	320
#define LCD_LINE_EMPTY	1	//字符之间隔一个像素

//返回字体类型
static FONT_TYPE_T get_word_type(FONT_SIZE_T size, unsigned char is_hz) {
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
    fprintf(stderr, "word type is %d\t", ret);
    return ret;
}

//row, line始终指向下一个空白位置,可能换行也可能跳到行首
static void cal_lca(FONT_TYPE_T font_type, int *row, int *lines) {
    int row_1 = *row, line_1 = *lines;
    int font_size_r, font_size_l; 
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

    row_1 += font_size_r + LCD_LINE_EMPTY;
    if (row_1 >= LCD_ROW) {
	row_1 = 0;
	line_1 += font_size_l + LCD_LINE_EMPTY;
	if (line_1 >= LCD_LINE)
	    line_1 = 0;
    }

    *row = row_1;
    *lines = line_1;
}

////从字库中取出当前字的点阵, 并返回总共的字节数
static void SST25_R_BLOCK(int offset, unsigned char *bit_buf, int len) {
    FILE *fp = fopen("new_font.bin", "r");
    if (fp == NULL) {
	fprintf(stderr, "open font.bin error.\n");
    }
    fseek(fp, offset, SEEK_SET);
    if (fread(bit_buf, 1, len, fp) < len)
	fprintf(stderr, "fread %s error, try to read %d byte, offset %d.\n", "font.bin", len, offset);
    else 
	fprintf(stderr, "OK. try to read %d byte.\n",  len);
    fclose(fp);
}

static int get_bitmap(FONT_TYPE_T font_type, unsigned char *bit_buf, const unsigned char *str) {
    int offset, len = 0;

    switch (font_type) {
	case ASC_12:
	    len = 12 * 1;	//12的字库，实际上是12 *8 bit的
	    offset = ASC_12_OFFS + (*str) * len;			
	    break;
	case ASC_16:
	    len = 16 * 1;	//16 * 8
	    offset = ASC_16_OFFS + (*str) * len;			
	    break;
	case ASC_24:
	    len =  24 * 2; //24 * 16
	    offset = ASC_24_OFFS + (*str) * len;			
	    break;
	case HZK_16:
	    len = 16 * 2;
	    offset = HZK_16_OFFS + (94*(str[0] - 0xa0 -  1) + (str[1] - 0xa0 -1)) * len;			
	    break;
	case HZK_24:
	    len = 24 * 3;
	    offset = HZK_24_OFFS + (94*(str[0] - 0xa0  - 15 - 1) + (str[1] - 0xa0 -1)) * len;			
	    break;
	default:
	    break;	
    }

    SST25_R_BLOCK(offset, bit_buf, len);
    return len;
}

//从低位到高位，位为0表示描背景色，位为1表示为字体颜色；
static void send_8bit(FONT_TYPE_T font_type, unsigned char tmp) {
    int i;
    static int j = 0;
    int flag = 0;

    for (i = 7; i > 0; i--)
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

static void set_lcd_row_line(FONT_SIZE_T size, int row, int lines) {
    //extern void LCD_Lib_SetXY(unsigned short sax, unsigned short say, unsigned short eax, unsigned short eay);
    //LCD_Lib_SetXY(lines, row, lines + size, row + size);
    printf("\n");
}

void lcd_print(FONT_SIZE_T size, int row, int lines, const unsigned char *str) {
    unsigned char tmp;		//指向当前字符
    char is_hz;
    unsigned char bit_buf[FONT_MAX * (FONT_MAX/8)];
    int i, j;
    FONT_TYPE_T font_type;

    while (*str != '\0') {
	is_hz = (*str) > 0xa0 ? 1 : 0;	//判断是否为汉字
	//设置屏幕输出的起始位置
	set_lcd_row_line(size, row, lines);

	//返回字体类型
	font_type = get_word_type(size, is_hz);

	//从字库中取出当前字的点阵, 并返回总共的字节数
	j = get_bitmap(font_type, bit_buf, str);

	for (i = 0; i < j; i++) 
	    //从低位到高位，位为0表示描背景色，位为1表示为字体颜色；
	    send_8bit(font_type, bit_buf[i]);

	//row, line始终指向下一个空白位置,可能换行也可能跳到行首
	str = is_hz ? str + 2 : str + 1;	//指向下一个字符
	cal_lca(font_type, &row, &lines);
    }

}

int main(void) {
    unsigned char tmp[] = {0xCE, 0xD2, 0xC3, 0xC7, 0x00 }; //汉字GB2312 "我们"
    lcd_print(FONT_12, 0, 0, "abced");
    lcd_print(FONT_16, 0, 0, "abced");
    lcd_print(FONT_24, 0, 0, "abced");
    lcd_print(FONT_16, 0, 0, tmp);
    lcd_print(FONT_24, 0, 0, tmp);
    return 0;
}
