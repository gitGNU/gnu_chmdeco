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
	"<meta name=\"GENERATOR\" content=\"" PROGNAME " " VERSION"\">\r\n" \
	/* FIXME: Add a meta with the url to the website? */ \
	"<!-- Sitemap 1.0 -->\r\n" \
	"</HEAD><BODY>\r\n"
#define SITEMAP_FOOTER "</UL>\r\n</BODY></HTML>\r\n"

#endif /* _SITEMAP_H_ */
