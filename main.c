#include "bmptoico.h"		//for the requisite functions ReadBMP and WriteBMPtoICO
#include <stdio.h>
#include <stdlib.h>		//for free function

int main()
{
	byte *pixels;		//stores the pixel array of .bmp file
	byte *colors;		//stores the color table of 8 bit .bmp file
	int32 width;		//stores the width of .bmp file
	int32 height;		//stores the height of .bmp file
	int16 bitsPerPixel;	//stores the bpp of .bmp file
	int32 resx;		//stores the resolutions of the .bmp file
	int32 resy;
	char path[100];
	scanf("%s",path);
	ReadBMP(path, &pixels, &colors, &width, &height, &bitsPerPixel, &resx, &resy);				//Read the .bmp file
	WriteBMPtoICO("destination.ico", pixels, colors, width, height, bitsPerPixel, resx, resy);		//Write contents of .bmp file to .ico file
	//free the pixel array and color table memory blocks(to avoid memory leak)
	free(pixels);	
	free(colors);
	return 0;
}
