#ifndef BMPTOICO_H
#define BMPTOICO_H

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;


void ReadBMP(const char *fileName,byte **pixels,byte **colors,int32 *width, int32 *height, int16 *bitsPerPixel,int32 *resx,int32 *resy);//func to read .bmp file

void WriteBMPtoICO(const char *fileName, byte *pixels,byte *colors, int32 width, int32 height,int16 bitsPerPixel,int32 resx,int32 resy);//func to create .ico file and write the bmp to ico file

#endif
