#include "sdoksolv.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int found( char **, int *, int *);	//	find next empty cell 
int ok( char **, int, int);			//	check if cell is valid 
int sudoku( bmp *, char **);		//	recursively solve 

int main( int argc, char **argv) {
	char *out = (char*)malloc(50);	//	output file name 
	UNUSED( argc);
	bmp *map;						//	bitmap object 
	
	if( *argv[2] == '1') {
		map = jToBmap( argv[1]);				//	reading the file, converting to bitmap 
		sprintf( out, "output_task1_%.*s.bmp", 
					  7, argv[1] + 14);
		writeBmap( map, out);					//	task 1: write bitmap
		
		mirror( map);							//	mirror each 5x5 cell
		sprintf( out, "output_task2_%.*s.bmp", 
					  7, argv[1] + 14);
		writeBmap( map, out);					//	task 2: correct the input
		
		sprintf( out, "output_task3_%.*s.json", 
					  7, argv[1] + 14);
		FILE *fout = fopen( out, "w");
		fprintf( fout, "{\n\t\"input_file\":\t\"%.*s\",\n\t\"game_state\":\t\"%s\"\n}\n", 
					   12, argv[1] + 14, (check( extract( map))?"Win!":"Loss :("));			//	task 3: write formatted output to file
		
		fclose( fout);
	} else {
		map = readBmap( argv[1]);
		
		if( *argv[2] == '4') {
			sprintf( out, "output_task4_%.*s.bmp", 
						  7, argv[1] + 14);
			simple( map); writeBmap( map, out);		//	solve simple sudoku
		} else {
			sprintf( out, "output_bonus_%.*s.bmp", 
						  7, argv[1] + 14);
			char **tab = extract( map);				//	convert bitmap to matrix
			
			if( !sudoku( map, tab))					//	if no solution found, prepare the output
				for( int i = 0; i < 9; ++i)
					for( int j = 0; j < 9; ++j)
						writeTo( map, i, j, 0);
						
			writeBmap( map, out);
			for( int i = 0; i < 9; ++i)				//	free the matrix
				free( tab[i]);
			free( tab);
		}
		
	}

	free( map -> data);
	free( map);
	free( out);
	return 0;
}

int found( char **tab, int *i, int *j) {
	for( *i = 0; *i < 9; ++(*i))
		for( *j = 0; *j < 9; ++(*j))
			if( !tab[*i][*j]) return 1;
	return 0;
}

int ok( char **tab, int i, int j) {
	for( int k = 0; k < 9; ++k)
		if( ( k != i && tab[i][j] == tab[k][j]) || 
			( j != k && tab[i][j] == tab[i][k])) return 0;

	int k = ( i/3)*3, 
		l = ( j/3)*3;

	for( int m = k; m < k + 3; ++m)
		for( int n = l; n < l + 3; ++n)
			if( i != m || j != n) if( tab[i][j] == tab[m][n]) 
				return 0;
				
	return 1;
}

int sudoku( bmp *map, char **tab) {
	static unsigned char mage[] = { ma};
	int i, j;

	if( !found( tab, &i, &j)) return 1;
		else for( int k = 1; k < 10; ++k) {
			tab[i][j] = k;
			
			writeTo( map, i, j, tab[i][j]);
			paint( map, i, j, mage);
			
			if( ok( tab, i, j))if( sudoku( map, tab)) return 1;
			
			tab[i][j] = 0;
	}
	return 0;
}
