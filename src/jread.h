#ifndef _J_READ_H_
#define _J_READ_H_

#define BUFL 2048

#include "bmp_header.h"

#pragma pack(1)	//	avoid padding

typedef struct {
	bmp_fileheader file;
	bmp_infoheader info;
} bmp_header;	//	bmp header info

typedef struct {
	bmp_header header;
	unsigned char *data;
} bmp;			//	bmp file structure

#pragma pack()

void swap( unsigned char *, unsigned char *);
unsigned char next_int( char *, int *);			//	extract next int from string
int findField( char *);							//	find index for bmp header information

int _jRead( char *, char **);					//	read .json file
void readJson(char *, char **, char **, int *, int *);	//	separate pixel data from header information

bmp *jToBmap( char *);							//	convert json file to bmp image
bmp *readBmap( char*);							//	read bitmap binary file
void writeBmap( const bmp *, char *);			//	write bitmap image to file
void mirror( bmp *);							//	correct input data

#endif	//	_J_READ_H_