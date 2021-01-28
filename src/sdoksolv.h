#ifndef _SDOK_SOLV_
#define _SDOK_SOLV_

#include "jread.h"

#define UNUSED(expr) do { (void)(expr); } while (0)	//	to silence a warning :)

#define ma 0xff, 0x00, 0xff	//	magenta
#define wh 0xff, 0xff, 0xff	//	white
#define pi 0xaf, 0xaf, 0xff	//	pink
#define re 0x00, 0x00, 0xff	//	red

int cntZeroS(char **, int, int);	//	count zeroes in square
int sumSq( char **, int, int);		//	sum elements in square

int cntZeroR(char **, int);			//	count zeroes in line
int sumRow( char **, int);			//	sum elements in line

int cntZeroC(char **, int);			//	count zeroes in column
int sumCol( char **, int);			//	sum elements in column

int check( char **);				//	check if solved

char memToNum( const char *);		//	convert from bitmap to number
char **extract( bmp *);				//	convert sudoku bitmap to matrix

void paint( bmp *, int, int, const unsigned char *);	//	change cell number colour
void writeTo( bmp *, int, int, int);					//	write number to cell
void simple( bmp *);									//	solve a simple sudoku

#endif	//	_SDOK_SOLV_