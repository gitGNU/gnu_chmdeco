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
hhk.c - this module implements recreating the .hhk file from the $WWKeywordLinks dir
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
#include "hhk.h"



FILE* btree = NULL;
FILE* hhk = NULL;
bool hhk_success = false;

BYTE block[2*2+3*4];

char* read_UCS_string(FILE* f,uint* len);

void recreate_hhk( void ){
	/* FIXME: go through the #WINDOWS file & look for a window type with an index file? */
	char* inf = Index_file ? Index_file : HHK_FILE_NAME;

	if( !open_sitemap() ) return;

	/* FIXME: really need a case-insensitive fopen */
	btree = fopen("$WWKeywordLinks/BTree","rb");
	if( !btree ) btree = fopen("$WWKeywordLinks/BTREE","rb");
	if( btree ){
		DWORD next, num_topics, index, lastdepth = 1;
		WORD space_left, block_size = 2048, free_space, See_Also;
		uint len; char* Name; char* See_Also_s;

		fseek(btree,0x4C,SEEK_SET);
		depth = 1;
		do{

			if( !fread(block,12,1,btree) ){
				fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "$WWKeywordLinks/BTree", strerror(errno) );
				break;
			}

			next = get_DWORD(block+8);
			free_space = get_WORD(block);
			/* num_entries = get_WORD(block+2); */
			/* prev = get_DWORD(block+4); */

			space_left = block_size - 12;
			while( space_left > free_space ){

				Name = read_UCS_string(btree,&len);
				space_left -= len*2;

				if( fread(block,16,1,btree) ){
					space_left -= 16;

					See_Also = get_WORD(block);
					lastdepth = depth;
					depth = get_WORD(block+2)+1;
					index = get_DWORD(block+4);
					num_topics = get_DWORD(block+0xC);

					if( !hhk ){
						hhk = recreate( inf );
						if( !hhk ){
							fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", inf, strerror(errno) );
							close_sitemap(); FREE(Name); FCLOSE( btree ); return;
						}

						fputs(SITEMAP_HEADER"<UL>\r\n",hhk);
					}

					/* Add enough ul tags to get to the right depth */
					if( lastdepth < depth ){
						DWORD i = depth;
						for( depth = lastdepth; depth < i; depth++ ){
							DEPTHPUT(hhk); fputs("<UL>\r\n",hhk);
						}
					} else if( lastdepth > depth ){
						DWORD i = depth;
						for( depth = lastdepth-1; depth >= i; depth-- ){
							DEPTHPUT(hhk); fputs("</UL>\r\n",hhk);
						}
						depth = i;
					}

					/* Start the entry */
					DEPTHPUT(hhk); fputs("<LI> <OBJECT type=\"text/sitemap\">\r\n",hhk);

					/* Print the name */
					if(Name){ DEPTHPUT(hhk); fprintf( hhk, "\t<param name=\"Name\" value=\"%s\">\r\n", Name+index ); FREE(Name); }

					/* Print the other param's */
					if(See_Also/*==2*/){
						See_Also_s = read_UCS_string(btree,&len);
						space_left -= len*2;
						if(See_Also_s){ DEPTHPUT(hhk); fprintf( hhk, "\t<param name=\"See Also\" value=\"%s\">\r\n", See_Also_s ); FREE(See_Also_s); }
					} else {
						uint topici;
						for(topici=0; topici < num_topics && space_left > free_space; topici++){
							if( !read_DWORD( btree, &index ) ){
								fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", inf, strerror(errno) );
								FCLOSE(hhk); FCLOSE( btree ); return;
							}
							print_topics_entry(hhk,index);
							space_left -= 4;
						}
					}

					/* End the entry */
					DEPTHPUT(hhk); fputs("\t</OBJECT>\r\n",hhk);

					/* Next one */
					fseek(btree,8,SEEK_CUR);
					space_left -= 8;
				} else
					fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "$WWKeywordLinks/BTree", strerror(errno) );
			}

			/* Go to the next block */
			fseek(btree,space_left,SEEK_CUR);

		}while( next != -1 );

		/* Go back to the root depth */
		while(--depth){
			DEPTHPUT(hhk);
			fputs("</UL>\r\n",hhk);
		}

		fputs( SITEMAP_FOOTER, hhk );

		if( ferror(btree) )
			fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "$WWKeywordLinks/BTree", strerror(errno) );
		FCLOSE( btree );

		if( ferror(hhk) )
			fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", inf, strerror(errno) );
		FCLOSE( hhk );

	} else if( errno && errno != ENOENT )
		fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "$WWKeywordLinks/BTree", strerror(errno) );

	close_sitemap();
}

#define GROW 50

/* No buffering for now, just read */
/* The buffer is always GROW bytes larger than it needs to be */
/* the UCS2 to ANSI converter only needs to drop \0, since the HHK is ANSI only
for the alinks it will be more complex - need to convert the string to whatever
the charset of the target filename is */
char* read_UCS_string(FILE* f,uint* len){
	char* t; char* start;
	char* ret = NULL;
	uint i,ii,bytes = 0;
	*len = 0;
	for(;;){
		/* Get more space */
		t = (char*)realloc(ret,bytes+GROW*2);
		if( !t ){
			fprintf( stderr, "%s: %s %s: %s\n", PROGNAME, input, "$WWKeywordLinks/BTree buffer", strerror(errno) );
			*len = 0; FREE(ret); return NULL;
		}

		ret = t; start = ret + bytes; bytes += GROW;

		/* Get more bytes */
		if( !fread(start,1,GROW*2,f) || ferror(f)){
			fprintf( stderr, "%s: %s %s: %s\n", PROGNAME, input, "$WWKeywordLinks/BTree", strerror(errno) );
			*len = 0; FREE(ret); return NULL;
		}

		/* Convert what we got */
		for(i=ii=0;i<GROW*2;i+=2,ii++){
			if(!start[i]&&!start[i+1]){
				start[ii]=0; *len += ++ii;
				fseek(f,-2*(signed)(GROW-ii),SEEK_CUR);
				return ret;
			}
			start[ii] = start[i];
		}
		*len = bytes;
	}
	return ret;
}
