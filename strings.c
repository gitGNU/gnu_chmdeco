/*
chmdeco -- extract files from ITS/CHM files and decompile CHM files
Copyright (C) 2003 Pabs

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA, 02111-1307, USA or visit:
http://www.gnu.org
*/



/*
strings.c - this module implements an interface to the #STRINGS file
It was written by Pabs.
*/



/* System headers */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>



/* Local headers */



#include "chmdeco.h"
#include "common.h"
#include "strings.h"

#define BUF_LEN 5000

FILE* strings = NULL;
off_t strings_len = 0;
BYTE string_buf[BUF_LEN+1];
size_t string_buf_len = 0;
off_t string_offset = 0;
off_t string_buf_offset = 0;

bool open_strings( void ){

	if( strings ) return true;

	errno = 0;
	strings = fopen( "#STRINGS", "rb" );
	if( strings ){

		string_buf[BUF_LEN] = 0;
		string_offset = string_buf_offset = 0;
		string_buf_len = fread(string_buf,1,BUF_LEN,strings);
		if( string_buf_len && !ferror(strings) ){

			/* Find out how long the file is */
			fseek(strings, 0, SEEK_END);
			strings_len = ftell(strings);
			fseek(strings, 0, SEEK_SET);

			if( strings_len != -1 ){
				atexit(close_strings);
				return true;
			}
		}
		FCLOSE(strings);
	}

	/* The #STRINGS file is essential, so always print */
	fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#STRINGS", strerror(errno) );

	return false;
}

bool print_string( FILE* f, off_t off ){

	if( off >= strings_len || off <= 0 ) return false;

	if( off < string_offset || off >= string_offset + string_buf_len ){
		string_buf_offset = 0;
		fseek( strings, string_offset = off, SEEK_SET );
		string_buf_len = fread(string_buf,1,BUF_LEN,strings);
	}

	{
		BYTE* start; BYTE* end; size_t length, bytes_left;
		for(;;){

			string_buf_offset = off - string_offset;
			start = string_buf + string_buf_offset;
			bytes_left = string_buf_len - string_buf_offset;

			if ( string_buf_len ){

				end = (BYTE*)memchr( start, 0, bytes_left );
				if( !end ) end = start + bytes_left;
				length = end - start;
				string_buf_offset += length+1;
				fwrite(start,length,1,f);

				if( end < string_buf+string_buf_len )
					return !ferror(f) && !feof(f) && !ferror(strings);
			}

			off = (string_offset += string_buf_len);
			string_buf_offset = 0;
			string_buf_len = fread(string_buf,1,BUF_LEN,strings);

			if( !string_buf_len ) return true;
			else if( ferror(strings) ){
				fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#STRINGS", strerror(errno) );
				return false;
			}
		}
	}
}

char* get_string( off_t off ){

	if( off >= strings_len || off <= 0 ) return NULL;

	if( off < string_offset || off >= string_offset + string_buf_len ){
		string_buf_offset = 0;
		fseek( strings, string_offset = off, SEEK_SET );
		string_buf_len = fread(string_buf,1,BUF_LEN,strings);
	}

	{
		char* ret = NULL; char* t; char* ret_start; size_t ret_length = 0;
		BYTE* start; BYTE* end; size_t length, bytes_left;
		for(;;){

			string_buf_offset = off - string_offset;
			start = string_buf + string_buf_offset;
			bytes_left = string_buf_len - string_buf_offset;
			if ( string_buf_len ){

				end = (BYTE*)memchr( start, 0, bytes_left );
				if( !end ) end = start + bytes_left;
				length = end - start;
				string_buf_offset += length+1;
				t = (char*)realloc( ret, ret_length+length+1 );

				if( t ){
					ret = t;
					ret_start = ret + ret_length;
					ret_length += length;
					strcpy( ret_start, start );
				} else {
					fprintf( stderr, "%s: %s %s: %s\n", PROGNAME, input, "#STRINGS buffer", strerror(errno) );
					FREE( ret ); return NULL;
				}

				if( end < string_buf+string_buf_len )
					return ret;
			}

			off = (string_offset += string_buf_len);
			string_buf_offset = 0;
			string_buf_len = fread(string_buf,1,BUF_LEN,strings);

			if( !string_buf_len ) return ret;
			else if( ferror(strings) ){
				fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#STRINGS", strerror(errno) );
				FREE( ret ); return NULL;
			}
		}
	}
}

/* Print the rest of the strings in the file */
void print_strings( FILE* f ){
	size_t bytes_left, length;
	BYTE* start;
	BYTE* end;

	do{

		bytes_left = string_buf_len;
		start = string_buf + string_buf_offset;
		if ( string_buf_len ) do{

			end = (BYTE*)memchr( start, 0, bytes_left );
			if( !end ) end = start + bytes_left;
			length = end - start;
			bytes_left -= length;
			string_buf_offset += length+1;

			fwrite(start,length,1,f);
			fputs("\r\n",f);

			start = ++end;

		} while ( end < string_buf+string_buf_len );

		string_buf_offset = 0;
		string_offset += string_buf_len;
		string_buf_len = fread(string_buf,1,BUF_LEN,strings);

	} while( string_buf_len );
}

void close_strings( void ){
	FCLOSE(strings);
}
