#include "sdoksolv.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned char digits[][3*5*5] = {	//	hard coded pixel configurations
	{ re, wh, wh, wh, re, wh, re, wh, re, wh, wh, wh, re, wh, wh, wh, re, wh, re, wh, re, wh, wh, wh, re},	//	x
	{ wh, wh, wh, wh, pi, wh, wh, wh, wh, pi, wh, wh, wh, wh, pi, wh, wh, wh, wh, pi, wh, wh, wh, wh, pi},	//	1
	{ pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi},	//	2
	{ pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi, pi},	//	3
	{ pi, wh, wh, wh, pi, pi, wh, wh, wh, pi, pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, wh, wh, wh, wh, pi},	//	4
	{ pi, pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi, pi},	//	5
	{ pi, pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi, pi, wh, wh, wh, pi, pi, pi, pi, pi, pi},	//	6
	{ pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, wh, wh, wh, wh, pi, wh, wh, wh, wh, pi, wh, wh, wh, wh, pi},	//	7
	{ pi, pi, pi, pi, pi, pi, wh, wh, wh, pi, pi, pi, pi, pi, pi, pi, wh, wh, wh, pi, pi, pi, pi, pi, pi},	//	8
	{ pi, pi, pi, pi, pi, pi, wh, wh, wh, pi, pi, pi, pi, pi, pi, wh, wh, wh, wh, pi, pi, pi, pi, pi, pi}	//	9
};

int cntZeroS( char **row, int i, int j) {
	int cnt = 0;
	int k = ( i/3)*3,
		l = ( j/3)*3;	//	round the indexes to nearest multiple of 3
		
	for( int m = k; m < k + 3; ++m)
		for( int n = l; n < l + 3; ++n)
			if( !row[m][n]) cnt++;
			
	return cnt;
}

int sumSq( char **row, int i, int j) {
	int sum = 0;
	int k = ( i/3)*3,
		l = ( j/3)*3;	//	round the indexes to nearest multiple of 3
		
	for( int m = k; m < k + 3; ++m)
		for( int n = l; n < l + 3; ++n)
			sum += row[m][n];
			
	return sum;
}

int cntZeroR(char **row, int i) {
	int cnt = 0;
	for( int j = 0; j < 9; ++j)
		if( !row[i][j]) cnt++;
	return cnt;
}

int sumRow( char **row, int i) {
	int sum = 0;
	for( int j = 0; j < 9; ++j)
		sum += row[i][j];
	return sum;
}

int cntZeroC(char **row, int j) {
	int cnt = 0;
	for( int i = 0; i < 9; ++i)
		if( !row[i][j]) cnt++;
	return cnt;
}

int sumCol( char **tab, int j) {
	int sum = 0;
	for( int i = 0; i < 9; ++i)
		sum += tab[i][j];
	return sum;
}

int check( char **tab) {
	for( int i = 0; i < 9; ++i)
		if( sumRow( tab, i) != 45) {	//	if not solved correctly, free matrix and return 0
			for( int j = 0; j < 9; ++j)
				free( tab[j]);
			free( tab);
			return 0;
		}
	
	for( int i = 0; i < 9; ++i)
		free( tab[i]);
	free( tab);
	return 1;							//	else return 1
}

char memToNum( const char *mem) {
	for( int i = 1; i < 10; ++i)
		if( !memcmp( *(digits + i), mem, 75))	//	comparing pinxels
			return i;
			
	return 0;
}

char **extract( bmp *map) {							//	convert sudoku image to matrix
	int w_len = (map -> header).info.width;			//	image width
	unsigned char *data = map -> data;				//	pointer to pixel data
	char *num = (char*)malloc(3*5*5);				//	5x5 squares with 3 bytes per pixel
	
	char **tab = (char**)malloc( 9*sizeof( char*));	//	allocate memory for matrix
	for( int i = 0; i < 9; ++i)
		tab[i] = (char*)malloc(9*sizeof(char));
	
	for( int i = 0; i < 9; ++i) {
		int row = i*8 + 2;							//	2px offset + 5px width + 3px borders
		
		for( int j = 0; j < 9; ++j) {
			int col = j*8 + 2;						//	2px offset + 5px width + 3px borders
			
			for( int m = 0; m < 5; ++m)				//	converting from 2d -> 1d => (x,y) = x*line_width + y
				memcpy( num + 3*5*m, data + 3*( (row+m)*w_len + col), 15);	//	times 3 to account for the pixel offset
			tab[i][j] = memToNum( num);
		}
	}
	
	free( num);
	return tab;
}

void writeTo( bmp *map, int i, int j, int d) {		//	write number to 5x5 cell
	int w_len = (map -> header).info.width;
	unsigned char *data = map -> data;
	unsigned char *num = digits[d];
	
	int row = 8*i + 2,
		col = 8*j + 2;
	
	for( int m = 0; m < 5; ++m)
		memcpy( data + 3*( (row+m)*w_len + col), num + 3*5*m, 15);
}

void paint( bmp *map, int i, int j, const unsigned char *clr) {		//	change number colour
	int w_len = (map -> header).info.width;
	unsigned char *data = map -> data;
	unsigned char whit[] = { wh};
	
	int row = 8*i + 2,
		col = 8*j + 2;
	
	for( int m = 0; m < 5; ++m)
		for( int n = 0; n < 5; ++n) {
			unsigned char *p = data + 3*( ( row + m)*w_len + ( col + n));
			if( memcmp( whit, p, 3)) memcpy( p, clr, 3);			//	check if different from white pixel configuration
		}
}

void simple( bmp *map) {
	char **tab = extract( map);
	unsigned char mage[] = { ma};
	
	for( int i = 0; i < 9; ++i) {
		for( int j = 0; j < 9; ++j) {
			if( !tab[i][j]) {
				int s = cntZeroS( tab, i, j),
					r = cntZeroR( tab, i);
				
				if( s == 1) tab[i][j] = 45 - sumSq( tab, i, j);
					else if( r == 1) tab[i][j] = 45 - sumRow( tab, i);
						else tab[i][j] = 45 - sumCol( tab, j);
				
				writeTo( map, i, j, tab[i][j]);
				paint( map, i, j, mage);
			}
				
		}
	}
	
	for( int i = 0; i < 9; ++i)
		free( tab[i]);
	free( tab);
}

