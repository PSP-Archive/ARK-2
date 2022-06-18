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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <psptypes.h>

#define	PSP_LINE_SIZE 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

typedef u32 Color;
#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))


typedef struct
{
	int textureWidth;  // the real width of data, 2^n with n>=0
	int textureHeight;  // the real height of data, 2^n with n>=0
	int imageWidth;  // the image width
	int imageHeight;
	Color* data;
} Image;


extern void clearScreen(Color color);

extern void putPixelScreen(Color color, int x, int y);

/**
 * Set a pixel in an image to the specified color.
 *
 * @pre x >= 0 && x < image->imageWidth && y >= 0 && y < image->imageHeight && image != NULL
 * @param color - new color for the pixels
 * @param x - left position of the pixel
 * @param y - top position of the pixel
 */

extern Color getPixelScreen(int x, int y);


void setPrintColorRGB(char r, char g, char b);

/**
 * Get the color of a pixel of an image.
 *
 * @pre x >= 0 && x < image->imageWidth && y >= 0 && y < image->imageHeight && image != NULL
 * @param x - left position of the pixel
 * @param y - top position of the pixel
 * @return the color of the pixel
 */

extern void printTextScreen(int x, int y, const char * text, u32 color);

void print_to_screen(const char * text);
extern void print_to_screen_color(const char * text, u32 color);
void (* PRTSTR)(const char *A, ...);

/**
 * Print a text (pixels out of the screen or image are clipped).
 *
 * @param x - left position of text
 * @param y - top position of text
 * @param text - the text to print
 * @param color - new color for the pixels
 * @param image - image
 */

extern Color* getVramDrawBuffer();

/**
 * Get the current display buffer for fast unchecked access.
 *
 * @return the start address of the current display buffer
 */
extern Color* getVramDisplayBuffer();

//clears screen with a given color
void SetColor();

void setBgColorRGB(char r, char g, char b);

void setPrintStateY(int y);
int getPrintStateY();

//init screen
void initScreen(int (*DisplaySetFrameBuf)());

//clear screen
void cls();

#endif
