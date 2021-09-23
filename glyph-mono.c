

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strlen

#include "namelist.c"

#include "glyph-read.c"

void spline_print_combo
(FILE* fp, unsigned char character, unsigned char* a_splineset[])
{
	if( character == 32 )
	{
		fprintf( fp, "0 0 m\n" );
		return;
	}
	fprintf( fp, "%s\n", a_splineset[(character & 3) | 12] );
	character >>= 2;
	fprintf( fp, "%s\n", a_splineset[(character & 3) | 8] );
	character >>= 2;
	fprintf( fp, "%s\n", a_splineset[(character & 3) | 4] );
	character >>= 2;
	fprintf( fp, "%s\n", a_splineset[character & 3] );
}

void produce_char
(FILE * fp, unsigned char character, unsigned char* a_splineset[])
{
	fprintf( fp, "\n" );
	if (ascii_names[character] == NULL)
	{
		fprintf( fp, "StartChar: uni00%X%X\n",
		         character >> 4, character & 15 );
	}
	else
		fprintf( fp, "StartChar: %s\n",
		         ascii_names [character] );
	fprintf( fp, "Encoding: %d %d %d\n",
	         character, character, character);
	fprintf( fp, "Width: 1024\n" );
	fprintf( fp, "Flags: H\n" );
	fprintf( fp, "LayerCount: 2\n" );
	fprintf( fp, "Fore\n" );
	
	fprintf( fp, "SplineSet\n" );
	spline_print_combo( fp, character, a_splineset );
	fprintf( fp, "EndSplineSet\n" );
	
	fprintf( fp, "EndChar\n" );
}


void load_splinesets( unsigned char* a_splineset[], FILE* fp_glyphs )
{
	int i;
	for (i = 0; i < 4; ++i)
	{
		printf( "Reading splineset %x\n", i );
		a_splineset[i] = load_next_splineset( fp_glyphs );
		printf( "%s\n", a_splineset[i] );
		a_splineset[i|4]  = get_shifted_string(a_splineset[i],0,400);
		a_splineset[i|8]  = get_shifted_string(a_splineset[i],400,0);
		a_splineset[i|12] = get_shifted_string(a_splineset[i],400,400);
	}
	printf( "Done reading glyphs from file\n" );
}

void dealloc_splinesets( unsigned char* a_splineset[] )
{
	int i;
	for (i = 0; i < 16; ++i)
	{
		//printf( "%s", a_splineset[i] );
		free( a_splineset[i] );
	}
}

int main
(int argc, char** argv)
{
	FILE * fp = fopen( "out.sfd", "w" );
	FILE * fp_header = fopen( "header.sfd-part", "r" );
	FILE * fp_glyphs = fopen( "glyph-in.sfd", "r" );

	unsigned char* a_splineset [32];

	unsigned char buffer;
	while( fread( &buffer, 1, 1, fp_header ) )
	{
		fwrite( &buffer, 1, 1, fp );
	}

	load_splinesets( a_splineset, fp_glyphs );
	unsigned char character = 0;
	for(;; ++character )
	{
		produce_char( fp, character, a_splineset );
		if( character == 255 )
			break;
	}
	fprintf( fp, "EndChars\nEndSplineFont\n" );
	dealloc_splinesets( a_splineset );
	fclose( fp );
	fclose( fp_header );
	fclose( fp_glyphs );
	return 0;
}
