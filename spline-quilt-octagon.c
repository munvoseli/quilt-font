#include <math.h>

static const unsigned char* coords_outer[] = {
	"0 512",
	"150 150",
	"512 0",
	"874 150",
	"1024 512",
	"874 874",
	"512 1024",
	"150 874",
	"0 512"
};

int qcos( int a ) // correct for a on [0, 8] ; {2, 6} -> 0
{
	if (a <= 2)
		return 128 * a * a - 512;
	else if (a <= 6)
		return 512 - 128 * (a - 4) * (a - 4);
	else
		return 128 * (a - 8) * (a - 8) - 512;
}

int qsin( int a )
{
	if( a <= 4 )
		return 512 - 128 * (a - 2) * (a - 2);
	else
		return 128 * (a - 6) * (a - 6) - 512;
}

void spline_print_octagon( FILE * fp, unsigned char character )
{
	if( character == ' ' )
	{
		fprintf( fp, "0 0 m 1\n" );
		return;
	}
	unsigned char i, mid_prev, mid_next;
	mid_prev = (character & 0x81) != 0x81;
	if (mid_prev)
		fprintf( fp, "%d 512 m 1\n", qcos(0) / 3 );
	else
		fprintf( fp, "0 512 m 1\n" );
	for (i = 0; i <= 8; ++i)
	{
		if (character == 0x58)
		{
			printf ("%X %X\n", character & (1 << i), mid_prev);
		}
		mid_next = (character >> (i % 8)) & 1;
		switch( (mid_next << 1) | mid_prev )
		{
		case 0: // both inner
			fprintf( fp, " %d %d l 1\n", qcos(i) / 4 + 512, qsin(i) / 4 + 512);
			break;
		case 1: // outer ring to inner ring
			fprintf( fp, " %d %d l 1\n", qcos(i)     + 512, qsin(i)     + 512);
			fprintf( fp, " %d %d l 1\n", qcos(i) / 4 + 512, qsin(i) / 4 + 512);
			break;
		case 2: // inner to outer
			fprintf( fp, " %d %d l 1\n", qcos(i) / 4 + 512, qsin(i) / 4 + 512);
			fprintf( fp, " %d %d l 1\n", qcos(i)     + 512, qsin(i)     + 512);
			break;
		case 3: // both outer
			fprintf( fp, " %d %d l 1\n", qcos(i)     + 512, qsin(i)     + 512);
			break;
		default:
			printf( "spline-quilt-octagon.c: ,,*-" );
		}
		mid_prev = mid_next;
	}
}
