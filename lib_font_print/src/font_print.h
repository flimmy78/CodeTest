#ifndef FONT_PRINT_H 
#define FONT_PRINT_H

#include <stdio.h>
#include <unistd.h>

typedef enum
{
	FP_FS_12 = 0,
	FP_FS_14,
	FP_FS_16,
	FP_FS_20,
	FP_FS_24,
	FP_FS_NUM
}FP_FontSize;

typedef enum
{
	FP_COLOR_AUTO = 0, /* �͵�ǰ��ɫ��ͬ */ 
	FP_COLOR_RED,
	FP_COLOR_LIGHT_RED,
	FP_COLOR_GREEN, 
	FP_COLOR_LIGHT_GREEN,
	FP_COLOR_BLUE,
	FP_COLOR_LIGHT_BLUE,  
	FP_COLOR_DARY_GRAY, 
	FP_COLOR_CYAN, 
	FP_COLOR_LIGHT_CYAN,  
	FP_COLOR_PURPLE,  
	FP_COLOR_LIGHT_PURPLE, 
	FP_COLOR_BROWN,  
	FP_COLOR_YELLOW,  
	FP_COLOR_LIGHT_GRAY,  
	FP_COLOR_WHITE,
	FP_COLOR_NUM
}FP_FontColor;

/*
��ӡ�ַ���
pString:	Ҫ��ӡ���ַ���
Meta:		��ʾ����Ԫ
WordSpace:�ּ��
FontSize:	�����С
FontColor:	������ɫ
*/
void FP_PrintString(char *pString, char Meta, unsigned int WordSpace, FP_FontSize FontSize, FP_FontColor FontColor);

#endif /* FONT_PRINT_H */
/* EOF */
