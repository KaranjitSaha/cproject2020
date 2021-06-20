#include <stdio.h>
#include <stdlib.h>//for malloc and all. “malloc” or “memory allocation” method in C dynamically allocate a group of memory with the specified size
#include <math.h>//for pow and all
#include "bmptoico.h"

//defining macros
#define DATA_OFFSET_OFFSET 0x000A 
#define WOffSet 0x0012
#define HOffSet 0x0016
#define BppOffSet 0x001C
#define pixpmHorizontalOffSet 0x0026
#define pixpmVerticalOffSet 0x002A
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0		//Defaults to 2^bitsperpix
#define ALL_COLORS_REQUIRED 0

//***Inputs*****
//fileName: The name of the file to save
//pixels: Pointer to the pixel data array
//colors: Pointer to the color table data array
//w: The w of the image in pixels
//height: The height of the image in pixels
//bpp: The number of bits per pixel that are used in the image
//pixpmhorizontal,pixpmvertical: The X resolution and Y resolution of the image in pixels/meter
void WriteBMPtoICO(const char *fileName, byte *pixels,byte *colors, int32 w, int32 h,int16 bpp,int32 pixpmhorizontal,int32 pixpmvertical)
{
        //Open file in binary mode
        FILE *outputFile = fopen(fileName, "wb");

	/******_ICONHEADER_******/
	
	//write reserved as 00 00
	int16 reserved=0x0000;
	fwrite(&reserved,2,1,outputFile);

	//write imageType 00 01 for .ico file
	int16 imageType=0x0001;
	fwrite(&imageType,2,1,outputFile);

	//write noOfImages(Here it is 1)
	int16 noOfImages=0x0001;
	fwrite(&noOfImages,2,1,outputFile);

	/****_ICONDIRECTORY_****/
	
	//write width of icon file(here it is width of img to be converted)
	byte wd=w;
	fwrite(&w,1,1,outputFile);

	//write height of icon file(here it is height of img to be converted)
	byte hgt=h;
	fwrite(&h,1,1,outputFile);

	//write color pallete(0 if bpp>=8)
	byte col=0x0000;
	fwrite(&col,1,1,outputFile);

	//write reserved as 00
	byte reserve=0x0000;
	fwrite(&reserve,1,1,outputFile);

	//write color planes=00 01 for .ico file
	int16 cplanes=0x0001;
	fwrite(&cplanes,2,1,outputFile);

	//write bitsPerPixel of bitmap file to be converted
	fwrite(&bpp,2,1,outputFile);

	//write the total filesize of .ico file
	int32 fileSize=INFO_HEADER_SIZE + pow(2,bpp) + (w*h*2);
	fwrite(&fileSize,4,1,outputFile);

	//write the bmp data offset
	int32 bmpDataOffset=0x0016;
	fwrite(&bmpDataOffset,4,1,outputFile);

        /*******_BITMAP_INFO_HEADER_******/
        
	//Write size
        int32 infoHeaderSize = INFO_HEADER_SIZE;
        fwrite(&infoHeaderSize, 4, 1, outputFile);

        //Write width
        fwrite(&w, 4, 1, outputFile);

	//According to .ico specs,write twice the height of bmp file
	int32 doubleh=2*h;
        fwrite(&doubleh, 4, 1, outputFile);

        //Write color planes
        int16 planes = 1; //always 1
        fwrite(&planes, 2, 1, outputFile);

        //write bits per pixel
        fwrite(&bpp, 2, 1, outputFile);

        //write compression
        int32 cmprsn = NO_COMPRESION;
        fwrite(&cmprsn, 4, 1, outputFile);

        //write image size (in bytes = bpp/8)
        int32 imgSize = w*h*bpp/8;
        fwrite(&imgSize, 4, 1, outputFile);

        //write resolution (in pixels per meter)
        int32 Xres = pixpmhorizontal; 
        int32 Yres = pixpmvertical; 
        fwrite(&Xres, 4, 1, outputFile);
        fwrite(&Yres, 4, 1, outputFile);
        
	//write colors used 
        int32 clrsUsed = MAX_NUMBER_OF_COLORS;
        fwrite(&clrsUsed, 4, 1, outputFile);
        
	//Write important colors
        int32 impColors = ALL_COLORS_REQUIRED;
        fwrite(&impColors, 4, 1, outputFile);
	
	//If file is 1-bit,4-bit or 8-bit then write its color table to .ico file
	if(bpp<= 8)
	{	
		int numberOfColors= pow(2,bpp);
		fwrite(colors, 1, 4*numberOfColors,outputFile);
	}
        
	//write data
        int i = 0;
	//calculate padded and unpadded row sizes(unpadded for pixel array data access and padded for writing)
        int paddedRSize = (int)(4 * ceil((float)(w) / 4.0f))*(bpp/8);
        int unpaddedRSize = w*bpp/8;
	//write the image data to the .ico file
        for ( i = 0; i < h; i++)
        {
                int pOffset = ((h - i) - 1)*unpaddedRSize;
                fwrite(&pixels[pOffset], 1, paddedRSize, outputFile);
        }
	
	
	//if bits of image < 32, the AND mask is supplied for transparency and inversion effects
	if (bpp < 32)
	{	
		//works for 8 bpp ,other sizes coming soon.
		//P.S. transparency is represented by one bit and hence the chosen bit will be either fully transparent or fully opaque
		byte mask=0x0000;
		int k=7;
		for(i=0 ; i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				//since we are not given the pixel of transperancy,we assume the entire image data to be copied opaque.
				//hence below image data the mask is set as 0(for full opaque copy).(the mask sum part is commented for this reason
				//by default,we assume 255(white) to be transparent and the rest to be opaque.we can change colors as per image to be                                       //converted.
				//mask += (pixels[j+width*(height-i-1)] == 255)?pow(2,k):0; 	//to make image transparent,uncomment the mask+ part
				k--;
				if(k==-1)
				{
					//each mask byte here repesents the AND mask bit data of 8 pixels
					k=7;			//the instant k==-1,the mask byte is written as data not stored bit-by-bit but byte-by-byte
					fwrite(&mask,1,1,outputFile);
					mask = 0x0000;
				}
			}

		}	
		//as in a .ico file,dimensions of mask == dimensions of image data,fill the rest of .ico file with 0x0000
		//does not affect data inside .ico file,just for validation purposes such that windows can open it.
		mask=0x0000;
		for(i = w*h/8;i<w*h;i++)
			fwrite(&mask,1,1,outputFile);
	}
        fclose(outputFile);
}


