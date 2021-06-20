#include <stdio.h>
#include <stdlib.h>//for malloc and all. “malloc” or “memory allocation” method in C is used to dynamically allocate a single large block of memory with the specified size
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


//using tydef to reduce our typing efforts
typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

//***Inputs*****
//fileName: The name of the file to open 
//***Outputs****
//pixels: A pointer to a byte array. This will contain the pixel data
//colors: A pointer to a byte array.This will contain the color table data(each element in BGR0 form)
//w: An int pointer to store the width of the image in pixels
//h: An int pointer to store the height of the image in pixels
//bpp: An int pointer to store the number of bits per pixel that are used in the image
//pixpmhorizontal: An int pointer to store the X Resolution of the image in pixels/meter
//pixpmvertical: An int pointer to store the Y Resolution of the image in pixels/meter
void ReadBMP(const char *fileName,byte **pixels,byte **colors,int32 *w, int32 *h, int16 *bpp,int32 *pixpmhorizontal,int32 *pixpmvertical)
{
        //Open the file for reading in binary mode
        FILE *imageFile = fopen(fileName, "rb");

	//Check for image existence
	if(imageFile == NULL)
	{
		printf("Unable to open source image,source image does not exist");
		exit(1);
	}

        //Read data offset
        int32 d0ffSet;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);	//fseek(filepointer,distance_to_move,starting_location)  , SEEK_SET-start of file
        fread(&d0ffSet, 4, 1, imageFile);

        //Read width
        fseek(imageFile, WOffSet, SEEK_SET);
        fread(w, 4, 1, imageFile);

        //Read height
        fseek(imageFile, HOffSet, SEEK_SET);
        fread(h, 4, 1, imageFile);

        //Read bits per pixel
        fseek(imageFile, BppOffSet, SEEK_SET);
        fread(bpp, 2, 1, imageFile);

	//Read Xresolution
	fseek(imageFile,pixpmHorizontalOffSet,SEEK_SET);
       	fread(pixpmhorizontal,4,1,imageFile);

	//Read Yresolution
	fseek(imageFile,pixpmVerticalOffSet,SEEK_SET);
	fread(pixpmvertical,4,1,imageFile);

	//Read color Table if bitsPerPixel<=8
	if(*bpp<= 8)
	{	
		int nocolours = pow(2,*bpp);	//for 1 bpp,2 colors:4 bpp,16 colors:8 bpp,256 colors  
		*colors= (byte*)malloc(4*nocolours);	//allocating a color array of 4*noofColors bytes
		fseek(imageFile, HEADER_SIZE + INFO_HEADER_SIZE,SEEK_SET);
		fread(*colors, 1, 4*nocolours,imageFile);
	}

        //Rows are stored bottom-up
        //Each row is padded to be a multiple of 4 bytes. 
        //We calculate the padded row size in bytes
        int paddedRSize = (int)(4 * ceil((float)(*w) / 4.0f))*(*bpp/8);	//ceil returns smallest integer greater than or equal to the number
        
	//We are not interested in the padded bytes, so we allocate memory just for
        //the pixel data
        int unpaddedRSize = (*w)*(*bpp/8);
        
	//Total size of the pixel data in bytes
        int imgsize = unpaddedRSize*(*h);

	//Allocating a pixel array
        *pixels = (byte*)malloc(imgsize);

        //Read the pixel data Row by Row.
        //Data is padded and stored bottom-up
        int i = 0;
        //point to the last row of our pixel array (unpadded)
        byte *crp = *pixels+((*h-1)*unpaddedRSize);
        while(i < *h)
        {
                //put file cursor in the next row from top to bottom
	        fseek(imageFile, d0ffSet+(i*paddedRSize), SEEK_SET);
	        //read only unpaddedRowSize bytes (we ignore the padding bytes since padding is not part of the real image)
	        fread(crp, 1, unpaddedRSize, imageFile);
	        //point to the next row (from bottom to top)
	        crp -= unpaddedRSize;
			i++;
        }

        fclose(imageFile);
}


