#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "jread.h"

void swap( unsigned char *a, unsigned char *b) {
	char x = *a;
	*a = *b;
	*b =  x;
}

unsigned char next_int( char *str, int *pos) {
	unsigned char num;
	while( 0x30 > str[*pos] || str[*pos] > 0x39)			//	while not character, advannce
		++(*pos);
	
	num = str[(*pos)++]^0x30;					//	assign first digit
	while( 0x2F < str[*pos] && str[*pos] < 0x3A) {
		num = (num<<3) + (num<<1) + (str[(*pos)++]^0x30);	//	append next digit
	}
	
	return num;
}

int findField( char *key) {
	static char *keys[] = {		//	key values
		"signature", "file_size", "reserved", "offset",
		"size", "width", "height", "planes", "bit_count", "compression",  
		"image_size", "x_pixels_per_meter", "y_pixels_per_meter", 
		"colors_used", "colors_important"
	};
	
	int n = sizeof(keys)/sizeof(keys[0]);
		
	for(int i=0; i<n; ++i) {
		if( !strcmp( key, keys[i]))
			return i;
	}
	
	return -1;	//	key not found
}

int _jRead( char *file, char **json) {
	char *buf = (char*)malloc( BUFL);
	FILE *in = fopen( file, "r");
	
	int len, r;
	fgetc(in);
	
	*json = (char*)malloc( BUFL);
	len = fread( *json, 1, BUFL, in);
	
	while( (r = fread( buf, 1, BUFL, in)) > 0) {			//	read in BUFL size chunks
		*json = realloc( *json, len + r + 1);			//	make room
		strncpy( *json + len, buf, r);				//	append next chunk
		len += r;						//	update length
	}
	
	(*json)[--len] = '\0';						//	append null terminator
	fclose( in);
	free( buf);

	return len;
}

void readJson(char *file, char **data, char **info, int *data_len, int *info_len) {
	int tot_len = _jRead( file, data);			//	read file
	*data_len = strcspn( *data, "]");			//	search for end of pixel data
	
	*info_len = tot_len - *data_len - 1;			//	header information length
	*info = *data + *data_len + 1;				//	assign pointer to header info
	
	(*data)[*data_len] = '\0';				//	mark pixel data ending by null terminator
}

bmp *jToBmap( char *file) {
	bmp *bitmap = (bmp*)malloc( sizeof(bmp));
	
	char *data, *info, *p,
		 *delim = "{}\":,";
	int dlen, ilen, k, i;
	
	readJson( file, &data, &info, 
					&dlen, &ilen);
					
	p = strtok( info, delim);
	while( p != NULL) {
		k = findField( p);
		if( k != -1) {					//	if key found, extract information
			p = strtok( NULL, delim);
			
			switch(k) {
				case 0:
					(bitmap -> header).file.fileMarker1 = p[0];
					(bitmap -> header).file.fileMarker2 = p[1];
					break;
				case 1:
					(bitmap -> header).file.bfSize = atoi( p);
					break;
				case 2:
					(bitmap -> header).file.unused1 = 0;
					(bitmap -> header).file.unused2 = 0;
					break;
				case 3:
					(bitmap -> header).file.imageDataOffset = atoi( p);
					break;
				case 4:
					(bitmap -> header).info.biSize = atoi( p);
					break;
				case 5:
					(bitmap -> header).info.width = atoi( p);
					break;
				case 6:
					(bitmap -> header).info.height = atoi( p);
					break;
				case 7:
					(bitmap -> header).info.planes = atoi( p);
					break;
				case 8:
					(bitmap -> header).info.bitPix = atoi( p);
					break;
				case 9:
					(bitmap -> header).info.biCompression = atoi( p);
					break;
				case 10:
					(bitmap -> header).info.biSizeImage = atoi( p);
					break;
				case 11:
					(bitmap -> header).info.biXPelsPerMeter = atoi( p);
					break;
				case 12:
					(bitmap -> header).info.biYPelsPerMeter = atoi( p);
					break;
				case 13:
					(bitmap -> header).info.biClrUsed = atoi( p);
					break;
				case 14:
					(bitmap -> header).info.biClrImportant = atoi( p);
					break;
			}
			
		}
		
		p = strtok( NULL, delim);		//	get next key
	}
	
	bitmap -> data = (unsigned char*)malloc( (bitmap -> header).info.width*		//	width
						 (bitmap -> header).info.height*	//	height
						 (bitmap -> header).info.bitPix/8);	//	byte per pixel
	k = i = 0;
	while( data[k]) {
		(bitmap -> data)[i++] = next_int( data, &k);				//	extract pixel data
	}
	
	free( data);
	return bitmap;
}

bmp *readBmap( char* file) {
	bmp *map = (bmp*)malloc(sizeof(bmp));
	FILE *in = fopen( file, "rb");
	
	fread( &(map -> header), sizeof(bmp_header), 1, in);				//	read header info
	int pad_yes = ((3*(map -> header).info.width + 3)/4)*4,				//	width with padding
		pad_no  = 	3*(map -> header).info.width,				//	width without padding
		pad_dif = pad_yes - pad_no;						//	padding
		
	map -> data = (unsigned char*)malloc( pad_no*(map -> header).info.height);
	for( int i = (map -> header).info.height-1; i >= 0; --i) {
		fread( map -> data + i*pad_no, 1, pad_no, in);
		for( int j=0; j<pad_dif; j++)
			getc( in);							//	read padding
	}
	
	fclose( in);
	return map;
}

void writeBmap( const bmp *map, char *file) {
	FILE *out = fopen( file, "wb");
	fwrite( &(map -> header), sizeof(bmp_header), 1, out);			//	write header info
	
	int pad_yes = ((3*(map -> header).info.width + 3)/4)*4,
		pad_no  = 	3*(map -> header).info.width,
		pad_dif = pad_yes - pad_no;
		
	for( int i=(map -> header).info.height-1; i>-1; --i) {
		fwrite( map -> data + i*pad_no, 1, pad_no, out);		//	write pixel data
		for( int j=0; j<pad_dif; j++)
			putc( 0, out);						//	add padding
	}

	fclose(out);
}

//	( nr_linie * lungime_linie + coloana) * nr_byte_per_pixel + byte_offset
//	( nr_linie * lungime_linie + col_start + col_max - col_curent) * nr_byte_per_pixel + byte_offset

void mirror( bmp *map) {
	int w_len = (map -> header).info.width;
	unsigned char *data = map -> data;
	
	for( int i = 0; i < 9; ++i) {
		int row = i*8 + 2;
		
		for( int j = 0; j < 9; ++j) {
			int col = j*8 + 2;
			
			for( int m = row; m < row + 5; ++m)
				for( int n = col; n < col + 2; ++n)
					for( int k = 0; k < 3; ++k) {
						swap( &data[ 3*( m*w_len + n) + k], 
							  &data[ 3*( m*w_len + 2*col - n + 4) + k]);
					}
		}
	}

}
