/*
Author:
Email:
Section:
Date:

Purpose:

input:
output:
 */


void seek_after_next( FILE* fp, const unsigned char* str )
{
	unsigned int str_length = strlen( str );
	unsigned int i;
	unsigned char* buffer = malloc( str_length + 1 );
	int temp_char;
	buffer[str_length] = 0;
	
	while( strcmp(str,buffer) != 0)
	{
		for( i = 0; i < str_length - 1; ++i )
			buffer[i] = buffer[i + 1];
		temp_char = fgetc( fp );
		if( temp_char == EOF ) // minimal hau
			break;
		buffer[str_length - 1] = temp_char;
	}
}


// reads the next splineset from the file and returns the string pointer
unsigned char* load_next_splineset
( FILE* fp_glyphin )
{
	unsigned char* str;
	long spline_begin;
	long spline_close;
	long spline_length;

	// calculate length of string
	seek_after_next( fp_glyphin, "uni000" );
	seek_after_next( fp_glyphin, "SplineSet" );
	spline_begin = ftell( fp_glyphin );
	seek_after_next( fp_glyphin, "EndSplineSet" );
	// - 11: make sure it ends with a newline
	spline_close = ftell( fp_glyphin ) - 11;
	spline_length = spline_close - spline_begin;

	// allocate the string
	str = malloc( spline_length );

	// read spline into string
	fseek( fp_glyphin, spline_begin, SEEK_SET );
	fread( str, 1, spline_length - 1, fp_glyphin );

	return str;
} // end load_next_splineset


// reads the next series of data into the array
// skip to next non-whitespace character
// if it is started with a letter, go until newline or zero
// if it is started with a number, go until space
// return newline or space or zero
unsigned char load_next_data
( unsigned char a_chars[], unsigned char* splinesetp,
  unsigned long* p_pos )
{
	unsigned int pos = *p_pos, begin_pos, end_pos;
	unsigned char delimiter;
	// skip to next non-whitespace character
	while( splinesetp[pos] == ' ' || splinesetp[pos] == '\n' )
		++pos;
	if( splinesetp[pos] == 0)
		return 0;
	begin_pos = pos;

	// if it's a number, go to the next space
	// if it's a letter, go to the end of the line
	if( splinesetp[pos] >= '0' && splinesetp[pos] <= '9' )
		delimiter = ' ';
	else
		delimiter = '\n';

	while( splinesetp[pos] != delimiter )
		++pos;
	// pos is now on delimiter
	*p_pos = end_pos = pos;

	// now, put the characters in the array
	int i = 0;
	for( pos = begin_pos; pos < end_pos; ++pos, ++i )
	{
		a_chars[i] = splinesetp[pos];
	}
	a_chars[i] = 0; // and 0-terminate

	return delimiter;
}

unsigned char* get_shifted_string
( unsigned char* splinesetp_in, long xoff, long yoff )
{
	unsigned char a_chars [16];
	// since ssp_out will have lower y values than ssp_in,
	// the length of ssp_out <= the length of ssp_in
	// need to protect against negatives
	unsigned char* splinesetp_out = malloc(strlen( splinesetp_in ) + 1);

	unsigned long inpos = 0;
	unsigned long outpos = 0;
	unsigned char delim;
	char isOnY = 0;
	long num;
	while( delim = load_next_data( a_chars, splinesetp_in, &inpos ))
	{
		if( delim == ' ' ) // number
		{
			num = strtol( a_chars, NULL, 10 );
			if (isOnY)
			{
				if (num - yoff > 0)
					num -= yoff;
				else
					num = 0;
			}
			else
			{
				if (num - xoff > 0)
					num -= xoff;
				else
					num = 0;
			}
			//num = 1024 - num;
			sprintf( splinesetp_out + outpos, "%d ", num );
			isOnY ^= 1;
		}
		else // not a number
		{
			sprintf( splinesetp_out + outpos,
				 "%s\n", a_chars );
			isOnY = 0;
		}
		while( splinesetp_out[outpos] )
			++outpos;
	}
	return splinesetp_out;
}

/*

unsigned char* load_next_splineset
(FILE* fp_glyphs)
{
	unsigned char last_glyph = 0;
	unsigned char curr_glyph = 0;
	fpos_t fp_glyphpos;
	unsigned int splineset_size = 0;
	unsigned char* splinesetp;

	// skip past the byte-and-newline "comment"
	// for very easy comments
	fseek( fp_glyphs, 2, SEEK_CUR );
	
	// get how many bytes are in spline set
	fgetpos( fp_glyphs, &fp_glyphpos );
	for(;;)
	{
		curr_glyph = fgetc( fp_glyphs );
		
		if( curr_glyph == EOF
		    || (curr_glyph == 10 && last_glyph == 10) )
			break;
		++splineset_size;
		last_glyph = curr_glyph;
	}
	fsetpos( fp_glyphs, &fp_glyphpos );

	splinesetp = (unsigned char*) malloc( splineset_size + 1 );
	fread( splinesetp, 1, splineset_size, fp_glyphs );
	fseek( fp_glyphs, 1, SEEK_CUR );
	splinesetp[splineset_size] = 0;
	return splinesetp;
}

*/
