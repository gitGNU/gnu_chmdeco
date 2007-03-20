/*
chmdeco -- extract files from ITS/CHM files and decompile CHM files
Copyright (C) 2003 Pabs

This file is part of chmdeco; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

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
hhc.c - this module implements recreating the .hhc file from the #TOCIDX file
It was written by Pabs.
*/



/* Local headers */

#include "chmdeco.h"
#include "common.h"
#include "convert.h"
#include "sitemap.h"
#include "strings_file.h"
#include "hhc.h"



FILE* tocidx = NULL;
FILE* hhc = NULL;
bool print_tree( void );

void recreate_hhc( void ){
	bool got_text_site_properties = false;
	bool got_tree = false;
	bool got_valid_prop = false;

	/* FIXME: go through the #WINDOWS file & look for a window type with a contents file? */
	char* cf = Contents_file ? Contents_file : HHC_FILE_NAME;

	/* FIXME: when there is no HHC don't output it */
	hhc = recreate( cf );
	if( hhc ){

		char* ImageList; DWORD ImageType, Background, Foreground;
		char* Font; DWORD Window_Styles, ExWindow_Styles;
		char* FrameName; char* WindowName;
		FILE* idxhdr;

		/* Grab some sitemap properties */
		errno = 0;
		idxhdr = fopen("#IDXHDR","rb");
		if( idxhdr ){
			BYTE hhc_text_site_properties[11*4];
			fseek( idxhdr, 0x14, SEEK_SET );
			if( fread(hhc_text_site_properties,sizeof(hhc_text_site_properties),1,idxhdr) ){
				BYTE* b = hhc_text_site_properties;
				got_text_site_properties = true;
				ImageList = get_string( get_DWORD(b) );
				ImageType = get_DWORD(b+8);
				Background = get_DWORD(b+0xC);
				Foreground = get_DWORD(b+0x10);
				Font = get_string( get_DWORD(b+0x14) );
				Window_Styles = get_DWORD(b+0x18);
				ExWindow_Styles = get_DWORD(b+0x1C);
				FrameName = get_string( get_DWORD(b+0x24) );
				WindowName = get_string( get_DWORD(b+0x28) );
			} else
				fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#IDXHDR", strerror(errno) );
			FCLOSE( idxhdr );
		} else if( errno && errno != ENOENT )
			fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#IDXHDR", strerror(errno) );

		fputs(SITEMAP_HEADER,hhc);

		/* There is also Auto Generated, which cannot yet be determined*/
		/* if( Auto_TOC ) fputs( "\t<param name=\"Auto Generated\" value=\"Yes\">", hhc ); */
		/* FIXME: some of these are buggy when there was no TOC */
		if( got_text_site_properties && (FrameName || WindowName || ImageList || ExWindow_Styles != 0xFFFFFFFF || Window_Styles != 0xFFFFFFFF || Background != 0xFFFFFFFF || Foreground != 0xFFFFFFFF || ImageType || Font) ){
			got_valid_prop = true;
			fputs( "<OBJECT type=\"text/site properties\">\r\n", hhc );
				if( print_entity_refs ){
					if( FrameName ){
						fputs( "\t<param name=\"FrameName\" value=\"", hhc );
						print_with_entity_refs( hhc, FrameName );
						fputs( "\">\r\n", hhc );
						FREE(FrameName);
					}
					if( WindowName ){
						fputs( "\t<param name=\"WindowName\" value=\"", hhc );
						print_with_entity_refs( hhc, WindowName );
						fputs( "\">\r\n", hhc );
						FREE(WindowName);
					}
					if( ImageList ){
						fputs( "\t<param name=\"ImageList\" value=\"", hhc );
						print_with_entity_refs( hhc, ImageList );
						fputs( "\">\r\n", hhc );
						FREE(ImageList);
					}
				} else {
					if( FrameName ){ fprintf( hhc, "\t<param name=\"FrameName\" value=\"%s\">\r\n", FrameName ); FREE(FrameName); }
					if( WindowName ){ fprintf( hhc, "\t<param name=\"WindowName\" value=\"%s\">\r\n", WindowName ); FREE(WindowName); }
					if( ImageList ){ fprintf( hhc, "\t<param name=\"ImageList\" value=\"%s\">\r\n", ImageList ); FREE(ImageList); }
				}
				if( Background != 0xFFFFFFFF ) fprintf( hhc, "\t<param name=\"Background\" value=\"0x%x\">\r\n", Background );
				if( Foreground != 0xFFFFFFFF ) fprintf( hhc, "\t<param name=\"Foreground\" value=\"0x%x\">\r\n", Foreground );
				if( ExWindow_Styles != 0xFFFFFFFF ) fprintf( hhc, "\t<param name=\"ExWindow Styles\" value=\"0x%x\">\r\n", ExWindow_Styles );
				if( Window_Styles != 0xFFFFFFFF ) fprintf( hhc, "\t<param name=\"Window Styles\" value=\"0x%x\">\r\n", Window_Styles );
				if( ImageType ) fputs( "\t<param name=\"ImageType\" value=\"Folder\">\r\n", hhc );
				if( Font ){
					/* No \t for some reason */
					if( print_entity_refs ){
						fputs( "<param name=\"Font\" value=\"", hhc );
						print_with_entity_refs( hhc, Font );
						fputs( "\">\r\n", hhc );
					} else fprintf( hhc, "<param name=\"Font\" value=\"%s\">\r\n", Font );
					FREE(Font);
				}
			fputs( "</OBJECT>\r\n", hhc );
		}

		fputs("<UL>\r\n",hhc);

		/* Print the TOC tree, grabbing stuff from the topics files */
		errno = 0;
		tocidx = fopen("#TOCIDX","rb");
		if( tocidx ){
			DWORD tree;
			if( read_DWORD(tocidx,&tree) ){
				fseek( tocidx, tree, SEEK_SET );
				if( open_sitemap() ){
					depth = 1;
					/* WARNING: recursion */
					got_tree = print_tree();
					close_sitemap();
				}
			} else
				fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#TOCIDX", strerror(errno) );
			FCLOSE( tocidx );
		} else if( errno && errno != ENOENT )
			fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#TOCIDX", strerror(errno) );

		fputs( SITEMAP_FOOTER, hhc );

		FCLOSE( hhc );

	} else
		fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", cf, strerror(errno) );

	/* Nothing useful */
	if( !got_valid_prop && !got_tree ){
		chdir( "#recreated" );
		remove( cf );
		chdir( ".." );
	}
}

#define New  2
#define BOOK 4
#define NAME 8

BYTE entry[20];

/* Recurse through the binary toc, ret false if none printed */
bool print_tree( void ){
	bool ret = false;
	DWORD next = 0;
	do {
		if( fread(entry,20,1,tocidx) ){

			DWORD flags = get_DWORD(entry+4);
			DWORD topics_index = get_DWORD(entry+8);

			/* FIXME: If we didn't ignore offset to parent we may be able to make this a loop instead of recursive */
			/* DWORD parent = get_DWORD(entry+0xC); */

			next = get_DWORD(entry+0x10);

			DEPTHPUT(hhc); fputs("<LI> <OBJECT type=\"text/sitemap\">\r\n",hhc);

			if( flags & NAME )
				/* This does the work of printing the right topic name etc */
				ret = print_topics_entry(hhc,topics_index) || ret;
			else{
				char* Name = get_string(topics_index);
				if(Name){
					DEPTHPUT(hhc);
					if( print_entity_refs ){
						fputs( "\t<param name=\"Name\" value=\"", hhc );
						print_with_entity_refs( hhc, Name );
						fputs( "\">\r\n", hhc );
					} else fprintf( hhc, "\t<param name=\"Name\" value=\"%s\">\r\n", Name );
					FREE(Name);
				}
			}

			if( flags & New ){
				DEPTHPUT(hhc); fputs("\t<param name=\"New\" value=\"1\">\r\n",hhc);
			}

			DEPTHPUT(hhc); fputs("\t</OBJECT>\r\n",hhc);

			/* Possibly some children */
			if( flags & BOOK ){
				if( fread(entry,8,1,tocidx) ){
					DWORD child = get_DWORD(entry);
					if(child){
						/* Got children */
						fseek(tocidx,child,SEEK_SET);
						DEPTHPUT(hhc); fputs("<UL>\r\n",hhc); depth++;
						print_tree();
						depth--; DEPTHPUT(hhc); fputs("</UL>\r\n",hhc);
					}
				} else {
					next = 0;
					fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#TOCIDX", strerror(errno) );
				}
			}

			/* In case there were children */
			if(next) fseek(tocidx,next,SEEK_SET);

		} else {
			fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#TOCIDX", strerror(errno) );
			break;
		}

	} while(next);
	return ret;
}
