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

#ifndef _SITEMAP_H_
#define _SITEMAP_H_

extern uint depth;
#define DEPTHPUT(a) \
	{ \
		uint _i; \
		for( _i = 0; _i < depth; _i++ ) \
			fputs("\t",(a)); \
	}

bool open_sitemap();
bool print_topics_entry( FILE* f, size_t index );
char* get_urlstr(off_t off);
DWORD find_in_urltbl( DWORD majik );
void close_sitemap();

#define SITEMAP_HEADER \
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\r\n" \
	"<HTML>\r\n" \
	"<HEAD>\r\n" \
	"<meta name=\"GENERATOR\" content=\"" PACKAGE " " VERSION"\">\r\n" \
	"<!-- Sitemap 1.0 -->\r\n" \
	"</HEAD><BODY>\r\n"
#define SITEMAP_FOOTER "</UL>\r\n</BODY></HTML>\r\n"

extern FILE* topics;
extern FILE* urltbl;
extern FILE* urlstr;

#endif /* _SITEMAP_H_ */
