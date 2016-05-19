/*
	Primitive reader and writer of uncompressed targa files.
  
    This software is NOT freeware or public domain.

    (C) Bedrich Benes 2000
	beda@campus.ccm.itesm.mx
	http://paginas.ccm.itesm.mx/~beda

*/
#ifndef __TGAH__
#define __TGAH__
/*
LoadTGA
Supported formats: 8,24 and 32 uncompressed images.  
data is the data that is returned
x and y is the resolution that is returned as a parameter
d is color bit depth also returned and 
name is the filename
*/
unsigned char *LoadTGA (const char *name, int *x, int *y, int *d);

/*
SaveTGA
name is the filename
data is data to be saved
x and y is the resolution
d is color depth (8, 24, or 32)
*/
void SaveTGA(char *name, unsigned char *data, int x, int y, int d);

#endif
