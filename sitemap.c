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
sitemap.c - this module implements an interface to the #TOPICS, #URLTBL & #URLSTR files.
It was written by Pabs.
*/



/* System headers */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



/* Local headers */

#include "chmdeco.h"
#include "common.h"
#include "sitemap.h"
#include "strings.h"



FILE* topics = NULL;
FILE* urltbl = NULL;
FILE* urlstr = NULL;

uint depth = 0;

bool open_sitemap(){
	if( open_strings() ){

		errno = 0;
		topics = fopen( "#TOPICS", "rb" );
		if( !topics && errno && errno != ENOENT )
			fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#TOPICS", strerror(errno) );

		errno = 0;
		urltbl = fopen( "#URLTBL", "rb" );
		if( !urltbl && errno && errno != ENOENT )
			fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#URLTBL", strerror(errno) );

		errno = 0;
		urlstr = fopen( "#URLSTR", "rb" );
		if( !urlstr && errno && errno != ENOENT )
			fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#URLSTR", strerror(errno) );

		if( topics && urltbl && urlstr ){
			return true;
		} else {
			FCLOSE(topics);
			FCLOSE(urltbl);
			FCLOSE(urlstr);
		}
		/* Don't close_strings(); - needed elsewhere */
	}

	return false;
}

/* Should fit most #URLSTR strings in one go */
#define GROW 100

/* No buffering for now, just seek n read */
char* get_urlstr(off_t off){
	char* t; char* start;
	char* ret = NULL;
	uint ret_len = 0;

	fseek(urlstr,off,SEEK_SET);
	do{

		/* Get more space */
		t = (char*)realloc(ret,ret_len+GROW);
		if( !t ){
			fprintf( stderr, "%s: %s %s: %s\n", PROGNAME, input, "#URLSTR buffer", strerror(errno) );
			FREE(ret);
			return NULL;
		}

		ret = t; start = ret + ret_len; ret_len += GROW;

		/* Get more bytes */
		if( !fread(start,1,GROW,urlstr) || ferror(urlstr) ){
			fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#URLSTR", strerror(errno) );
			FREE(ret);
			return NULL;
		}

	}while(!memchr(start,0,GROW));
	return ret;
}

BYTE entry[4*4];

/* Print out the Name, Local, FrameName, URL of a topic */
bool print_topics_entry( FILE* f, size_t index ){

	fseek(topics, index*16, SEEK_SET);
	if( fread(entry,16,1,topics) ){

		DWORD urlstr_offset;

		char* Name = get_string(get_DWORD(entry+4));

		if(Name){ DEPTHPUT(f); fprintf( f, "\t<param name=\"Name\" value=\"%s\">\r\n", Name ); FREE(Name); }

		fseek( urltbl, get_DWORD(entry+8)+8, SEEK_SET);
		if( read_DWORD(urltbl,&urlstr_offset) ){

			fseek(urlstr, urlstr_offset, SEEK_SET);
			if( fread(entry,8,1,urlstr) ){

				DWORD URL, FrameName;
				char* URL_s = NULL;
				char* FrameName_s = NULL;

				char* Local = get_urlstr(urlstr_offset+8);
				if(Local){ DEPTHPUT(f); fprintf( f, "\t<param name=\"Local\" value=\"%s\">\r\n", Local ); FREE(Local); }

				URL = get_DWORD(entry);
				if(URL) URL_s = get_urlstr(URL);
				if(URL_s){ DEPTHPUT(f);  fprintf( f, "\t<param name=\"URL\" value=\"%s\">\r\n", URL_s ); FREE(URL_s); }

				FrameName = get_DWORD(entry+4);
				if(FrameName) FrameName_s = get_urlstr(FrameName);
				if(FrameName_s){ DEPTHPUT(f); fprintf( f, "\t<param name=\"FrameName\" value=\"%s\">\r\n", FrameName_s ); FREE(FrameName_s); }

				return true;
			} else fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#URLSTR", strerror(errno) );
		} else fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#URLTBL", strerror(errno) );
	} else fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#TOPICS", strerror(errno) );

	return false;
}

/* FIXME: When we know its meaning rename majik to something descriptive */
/* Search for an url with the specified majik number */
DWORD find_in_urltbl( DWORD majik ){
	/* Can't choose how to do this cause of that damn 4096 DWORD at the end */
	BYTE buf[4096]; size_t len; DWORD tmp = 0; BYTE* i;
	fseek( urltbl, 0, SEEK_SET );
	do{

		errno = 0;
		len = fread( buf, 1, 4096, urltbl );

		if( len < 4096 && len%12 )
			fprintf( stderr, "%s: warning: %s/%s: %s\n", PROGNAME, input, "#URLTBL", "partial entry found" );

		if( ferror(urltbl) || errno )
			fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#URLTBL", strerror(errno) );

		/* We only want complete entries */
		len = len/12*12;

		for(i=buf;i<&buf[len];i+=12){
			tmp = get_DWORD(i);
			if( tmp == majik ) return get_DWORD(i+8);
		}

	}while( len >= 4092 );
	return 0;
}

void close_sitemap(){
	/* Don't close_strings(); - needed elsewhere */
	FCLOSE(topics);
	FCLOSE(urltbl);
	FCLOSE(urlstr);
}
