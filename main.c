

#include <stdio.h>
#include <stdlib.h>

#include "namelist.c"

static const int coords_x [] = {
	0,
	0,
	512,
	1024,
	1024,
	1024,
	512,
	0,
	0
};

static const int coords_y [] = {
	512,
	1024,
	1024,
	1024,
	512,
	0,
	0,
	0,
	512
};

void spline_print_quilt
(FILE * fp, unsigned char character)
{
	unsigned char i;
	for (i = 0; i < 8; ++i)
	{
		if ( character & (1 << i) )
		{
			fprintf( fp, "512 512 m 1\n" );
			fprintf( fp, " %d %d l 1\n", coords_x [i], coords_y [i] );
			fprintf( fp, " %d %d l 1\n", coords_x [i + 1], coords_y [i + 1] );
			fprintf( fp, " 512 512 l 1\n" );
		}
	}
}

// The following code is less readable than spline_print_quilt, and both generate self-intersection errors, so I chose spline_print_quilt
void spline_print_fancy( FILE * fp, unsigned char character )
{
	unsigned char i, mid;
	mid = (character & 0x81) != 0x81;
	if (mid)
		fprintf( fp, "512 512 m 1\n" );
	else
		fprintf( fp, "0 512 m 1\n" );
	for (i = 0; i < 8; ++i)
	{
		if (character == 0x58)
		{
			printf ("%X %X\n", character & (1 << i), mid);
		}
		if ( character & (1 << i) )
		{
			if (mid)
				fprintf( fp, " %d %d l 1\n", coords_x [i], coords_y [i] );
			fprintf( fp, " %d %d l 1\n", coords_x [i + 1], coords_y [i + 1] );
		}
		else
		{
			if (!mid)
				fprintf( fp, " 512 512 l 1\n" );
		}
		mid = !(character & (1 << i));
	}
	if (!mid && !(character & 1))
		fprintf( fp, " 512 512 l 1\n" );
}

void produce_char
(FILE * fp, unsigned char character)
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
	spline_print_quilt( fp, character );
	fprintf( fp, "EndSplineSet\n" );
	
	fprintf( fp, "EndChar\n" );
}

int main
(int argc, char ** argv)
{
	FILE * fp = fopen ("out.sfd", "w");
	FILE * fp_header = fopen ("header.sfd-part", "r");

	unsigned char buffer;
	while (fread (&buffer, 1, 1, fp_header))
	{
		fwrite (&buffer, 1, 1, fp);
	}
	unsigned char character;
	for (character = 0;; ++character)
	{
		produce_char (fp, character);
		if (character == 255)
			break;
	}
	fprintf( fp, "EndChars\nEndSplineFont\n" );
	fclose (fp);
	fclose (fp_header);
	return 0;
}
