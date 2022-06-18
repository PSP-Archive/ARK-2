/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include "graphics.h"
#include "lib.h"
#include <pspdisplay.h>
#include <string.h>

#define IS_ALPHA(color) (((color)&0xff000000)==0xff000000?0:1)
#define FRAMEBUFFER_SIZE (PSP_LINE_SIZE*SCREEN_HEIGHT*4)
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define ABGR(a, b, g, r)	((((a)<<24) & 0xff000000) | (((b)<<16) & 0x00ff0000) | (((g)<<8) & 0x0000ff00) | ((r) & 0x000000ff))

Color* g_vram_base = (Color*) (0x40000000 | 0x04000000);

typedef union 
{
	int rgba;
	struct 
	{
		char r;
		char g;
		char b;
		char a;
	} c;
} color_t;

extern u8 msx[];

Color* getVramDrawBuffer()
{
	Color* vram = (Color*) g_vram_base;
	return vram;
}

Color* getVramDisplayBuffer()
{
	Color* vram = (Color*) g_vram_base;
	return vram;
}

void putPixelScreen(Color color, int x, int y)
{
	Color* vram = getVramDrawBuffer();
	vram[PSP_LINE_SIZE * y + x] = color;
}

Color getPixelScreen(int x, int y)
{
	Color* vram = getVramDrawBuffer();
	return vram[PSP_LINE_SIZE * y + x];
}

Color getPixelImage(int x, int y, Image* image)
{
	return image->data[x + y * image->textureWidth];
}

int gY = 0;
unsigned int gBgColor = 0x00000000;

void setBgColorRGB(char r, char g, char b)
{
	gBgColor = ABGR((unsigned int)255, (unsigned int)b, (unsigned int)g, (unsigned int)r);
}

void setPrintStateY(int y)
{
	gY = y;
}

int getPrintStateY()
{
	return gY;
}

void cls()
{
    gY = 0;
	int i;
	color_t *pixel = (color_t *)0x44000000;
	for(i = 0; i < 512*272; i++) {
		pixel->rgba = gBgColor;
		pixel++;
	}
}

void initScreen(int (*DisplaySetFrameBuf)())
{
	if(DisplaySetFrameBuf != NULL)
		DisplaySetFrameBuf((void *)0x44000000, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);

    cls();
}

void printTextScreen(int x, int y, const char * text, u32 color)
{
	int c, i, j, l;
	u8 *font;
	Color *vram_ptr;
	Color *vram;

	for (c = 0; c < strlen(text); c++) {
		if (x < 0 || x + 8 > SCREEN_WIDTH || y < 0 || y + 8 > SCREEN_HEIGHT) break;
		char ch = text[c];
		vram = getVramDrawBuffer() + x + y * PSP_LINE_SIZE;
		
		font = &msx[ (int)ch * 8];
		for (i = l = 0; i < 8; i++, l += 8, font++) {
			vram_ptr  = vram;
			for (j = 0; j < 8; j++) {
				if ((*font & (128 >> j))) *vram_ptr = color;
				vram_ptr++;
			}
			vram += PSP_LINE_SIZE;
		}
		x += 8;
	}
}

void print_to_screen_color(const char * text, u32 color) 
{
	if (gY > 272) 
	{
    	cls(0);
  	}
	
  	printTextScreen(0, gY, text, color);
  	gY += 12;
}


unsigned int gPrintColor = 0xFFFFFFFF;

void setPrintColorRGB(char r, char g, char b)
{
	gPrintColor = ABGR((unsigned int)255, (unsigned int)b, (unsigned int)g, (unsigned int)r);
}


void print_to_screen(const char * text) 
{
  	print_to_screen_color(text, gPrintColor);
}

void PRTSTR11(const char* A, unsigned long B, unsigned long C, unsigned long D, unsigned long E, unsigned long F, unsigned long G, unsigned long H, unsigned long I, unsigned long J, unsigned long K, unsigned long L)
{
  char buff[512];
  mysprintf11(buff, A, (unsigned long)B, (unsigned long)C, (unsigned long)D,  (unsigned long)E, (unsigned long)F, (unsigned long)G, (unsigned long)H,  (unsigned long)I, (unsigned long) J, (unsigned long) K, (unsigned long) L);
  print_to_screen(buff);
}

void (* PRTSTR)(const char* A, ...) = (void*)&PRTSTR11;
