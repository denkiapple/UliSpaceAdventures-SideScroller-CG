/*
	Primitive reader and writer of uncompressed targa files.

	This software is NOT freeware or public domain.
    
	(C) Bedrich Benes 2000
	beda@campus.ccm.itesm.mx
	http://paginas.ccm.itesm.mx/~beda

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "targa.h"


//This procedure reads data from the uncompressed tga file
//s is the file descriptor
//size is the size of the data and 
//bits is the bit color depth 
//it returns pointer to the correctly allocated data
unsigned char *ReadData(FILE *s, long int size, int bits)
{
    static unsigned char *data;
    unsigned char temp;
    int bread;
    int i;

    if (bits == 32) size = size*4;
	else
    if (bits == 24) size = size*3;
	else
	if  (bits == 8) size = size*1;
	else
	{
		printf("Unsuported targa color depth\n");
        exit(-1);
	}

    
	data = new unsigned char[size];
    if (data == NULL)
	{
		printf("Cannot allocate memory\n");
		exit(-1);
	}

    bread = fread(data, sizeof (unsigned char), size , s); 
    
    if (bread != size)
	{
		printf("Error reading data\n");
		exit(-1);
	}

    /* switch BGRA to RGBA */
    if (bits == 32) 
	{
     for (i = 0; i < size; i += 4) //skip by four
     {
         temp = data[i];
         data[i] = data[i + 2];
         data[i + 2] = temp;
     }
	}
    if (bits == 24) 
	{
     for (i = 0; i < size; i += 3) //skip by three
     {
         temp = data[i];
         data[i] = data[i + 2];
         data[i + 2] = temp;
     }
	}
    return data;
}



/*
LoadTGA
Supported formats: 8,24 and 32 uncompressed images.  
data is the data that is returned
x and y is the resolution that is returned as a parameter
d is color bit depth also returned and 
name is the filename
*/
unsigned char *LoadTGA (const char *name, int *x, int *y, int *d)
{
    unsigned char type[4];
    unsigned char info[7];
    unsigned char *imageData = NULL;
    int imageWidth, imageHeight;
    int imageBits;
	long int size;
    FILE *s;

    if (!(s = fopen(name, "rb"))) 
	{
		printf("Cannot open file %s\n",name);
		exit(-1);
	}

    fread(&type, sizeof (char), 3, s); // read in colormap info and image type, byte 0 ignored
    fseek(s, 12, SEEK_SET);			// seek past the header and useless info
    fread(&info, sizeof (char), 6, s);

    if (type[1] != 0 || (type[2] != 2 && type[2] != 3))
	{
		printf("Unsuported targa format or unrecognized header\n");
        exit(-1);
	}

    imageWidth  = info[0]+info[1]*256; 
    imageHeight = info[2]+info[3]*256;
    imageBits   = info[4]; 

    size = imageWidth*imageHeight; 
    
	imageData = ReadData(s,size,imageBits);

    /* no image data */
    if (imageData == NULL)
	{
		printf("Error reading targa file\n");
        exit(-1);
	}
    fclose (s);

	*x = imageWidth;
	*y = imageHeight;
	*d = imageBits;
    return imageData;
}

/*
SaveTGA
name is the filename
data is data to be saved
x and y is the resolution
d is color depth (8, 24, or 32)
*/
void SaveTGA(char *name, unsigned char *data, int x, int y, int d)
{ 
  FILE *f; 
  long int i;
  unsigned char buf[18]; //tga header
  unsigned char b[3]; //tmp buffer for writing

  f=fopen(name,"wb");
  if (f ==NULL) 
  { 
	  printf("Cannot create file %s\n",name);
	  exit(-1);
  }
    // create Targa-header 
   buf[0]=0;             // no image-ID field 
   buf[1]=0;             // color map is not included 
   buf[2]=2;             // uncompressed targa without color map 
   buf[3]=0;buf[4]=0;    // color map offset 0 - map is not here
   buf[5]=0;buf[6]=0;    // no colormap
   buf[7]=24;            // 24 Bit color 
   buf[8]=0;buf[9]=0;    // X-Position  
   buf[10]=0;buf[11]=0;  // Y-Position 
   buf[13]=(x>>8)&255;buf[12]=x&255;     // X-Size 
   buf[15]=(y>>8)&255;buf[14]=y&255;     // Y-Size 
   buf[16]=24; //24 bit per pixel	
   buf[17]=0; // 8 bit color  
   fwrite(&buf,18,1,f); 

   // save the data, but swap RGB to BGR 
    for (i=0;i<x*y;i++)
	{
      b[0] = data[3*i+2];
      b[1] = data[3*i+1];
      b[2] = data[3*i];
      if (fwrite(b,1,3,f) != 3)
	  {
		printf("Error writing data\n");
		exit(-1);
	  }
	}
    fclose(f); 
} 


/*
TGA header specification

From: 'Derek A. Benner' <dbenner@pacbell.net>

OK, Straight from 'Graphics File Formats, 2nd Edition' by David C. Kay & John R. Levine, here is the header format for the Targa image file. 

Offset          Length (in bytes)       Description
------          -----------------       -----------
0               1                       ID Field Length
1               1                       Color-map Type
2               1                       Image Type

        (Color-map-specific Info)
3               2                       First Color-map Entry
5               2                       Color-map Length
7               1                       Color-map Entry Size

        (Image-specific Info)
8               2                       Image X Origin
10              2                       Image Y Origin
12              2                       Image Width
14              2                       Image Height
16              1                       Bits-Per-Pixel
17              1                       Image-Descriptor Bits
For True-color images the value of Color-map Type should be 0, while color-mapped images should set this to 1. If a color map is present, then Color-map Entry Size should be set to 15, 16, 24 or 32. For 15 and 16 values each color map entry is stored in two bytes in the format of: 

High byte    Low byte  
A RRRRR GG   GGG BBBBB  
with the 'A' bit set to 0 for 15-bit color values. 24-bit-sized entries are stored as three bytes in the order of (B)lue, (G)reen, and (R)ed. 32-bit-sized color map entries are stored in four bytes ordered as (B)lue, (G)reen, (R)ed and (A)ttribute values. 

Further, the Image Type code should contain one of the following values: 

Code            Description
----            -----------
0               No Image Present
1               Color-mapped, uncompressed
2               True-color, uncompressed
3               Black-&-White, uncompressed
9               Color-mapped, RLE compressed
10              True-color, RLE compressed
11              Black-&-White, RLE compressed
The Image X & Y Origins and the Image Width & Height fields are self-explanatory. Bits-Per-Pixel holds the number of bits per image pixel and should hold the values 8, 16, 24, or 32. 

The Image Descriptor byte contains several bit fields that need to be extracted: 

Bits            Description
----            -----------
0-3             Attribute Bits (Explained later)
4               Left-to-Right orientation 0=L/R 1=R/L
5               Top/Bottom orientation 0=B/T 1=T/B
6-7             Scan-Line Interleaving 00H=None, 40H=2way, 80H=4way
The Attribute bits are used to define the attributes of the colors in color-mapped or true-color pixels. 0 means no alpha data, 1 means undefined and ignorable, 2 means undefined but should be preserved, 3 means regular alpha data and 4 means the pixel information has already been multiplied by the alpha value. 

Version 2.0 Targa files also have a file footer that may contain additional image and comment information. A version 2.0 Targa file always ends with the null-terminated string 'TRUEVISION-TARGA.'. So, if your Targa image ends with the values 'TRUEVISION-TARGA.' + 00H then you can extract the eight bytes prior to the string to find the start of the extension area and the developer directory positions within the file. Basically the Version 2.0 footer takes the format: 

Byte            Length          Description
-----           ------          -----------
0               4               32-bit offset to Extension Area
4               4               32-bit offset to Developer Directory
8               17              TRUEVISION-TARGA.
25              1               Binary zero ($0)
I'm not going to give complete descriptions to the Developer's Directory or the Extension Area. Instead, I'm going to point out the postage-stamp info that the V2.0 Targa file *MAY* contain. This postage stamp is a miniature of the image, no larger than 64 X 64 pixels in size, *IF PRESENT*! 

Extension Area 

Offset          Length          Description
------          ------          -----------
0               2               Extension Area Size (should be 495)
2               41              Author's Name
43              81              Author's Comments
124             81              Author's Comments
205             81              Author's Comments
286             81              Author's Comments
367             2               Creation Month
369             2               Creation Day
371             2               Creation Year
....            ...             ...
482             4               Color-correction table file offset
486             4               Postage-Stamp Image File Offset  ******
490             4               Scan-line table file offset
494             1               Attribute byte
This postage-stamp image, if present, may be directly usable by you. It is an uncompressed image in the same color format (Color-mapped or True-color) as the full image. 

*/
