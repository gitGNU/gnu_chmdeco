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
convert.c - this module implements some conversion functions
It was written by Pabs.
*/



/* System headers */

#include <sys/types.h>
#include <time.h>



/* Local headers */

#include "common.h"



struct lcid_string{
	LCID lcid;
	char* lcid_string;
} lcid_strings[] = {

/*
FIXME: Audit this list, remove crap, add more languages, fix names,
decide on "lang (country)" or "lang (countrian)"
*/
	{ 0x0000, "Language Neutral" },
	{ 0x0009, "English" },
	{ 0x0400, "Process Default Language" },
	{ 0x0401, "Arabic (Saudi Arabia)" },
	{ 0x0402, "Bulgarian" },
	{ 0x0403, "Catalan" },
	{ 0x0404, "Chinese (Taiwan)" },
	{ 0x0405, "Czech" },
	{ 0x0406, "Danish" },
	{ 0x0407, "German (Standard)" },
	{ 0x0408, "Greek" },
	{ 0x0409, "English (United States)" },
	{ 0x040A, "Spanish (Traditional Sort)" },
	{ 0x040B, "Finnish" },
	{ 0x040C, "French (Standard)" },
	{ 0x040D, "Hebrew" },
	{ 0x040E, "Hungarian" },
	{ 0x040F, "Icelandic" },
	{ 0x0410, "Italian (Standard)" },
	{ 0x0411, "Japanese" },
	{ 0x0412, "Korean" },
	{ 0x0413, "Dutch (Standard)" },
	{ 0x0414, "Norwegian (Bokmål)" },
	{ 0x0415, "Polish" },
	{ 0x0416, "Portuguese (Brazilian)" },
	{ 0x0417, "Rhaeto-Romanic" },
	{ 0x0418, "Romanian" },
	{ 0x0419, "Russian" },
	{ 0x041A, "Croatian" },
	{ 0x041B, "Slovak" },
	{ 0x041C, "Albanian" },
	{ 0x041D, "Swedish" },
	{ 0x041E, "Thai" },
	{ 0x041F, "Turkish" },
	{ 0x0420, "Urdu" },
	{ 0x0421, "Indonesian" },
	{ 0x0422, "Ukrainian" },
	{ 0x0423, "Byelorussian" },
	{ 0x0424, "Slovenian" },
	{ 0x0425, "Estonian" },
	{ 0x0426, "Latvian" },
	{ 0x0427, "Lithuanian" },
	{ 0x0429, "Farsi" },
	{ 0x042A, "Vietnamese" },
	{ 0x042D, "Basque" },
	{ 0x042E, "Sorbian" },
	{ 0x042F, "Macedonian" },
	{ 0x0430, "Sutu" },
	{ 0x0431, "Tsonga" },
	{ 0x0432, "Tswana" },
	{ 0x0433, "Venda" },
	{ 0x0434, "Xhosa" },
	{ 0x0435, "Zulu" },
	{ 0x0436, "Afrikaans" },
	{ 0x0438, "Faeroese" },
	{ 0x0439, "Hindi" },
	{ 0x043A, "Maltese" },
	{ 0x043B, "Sami (Lapland)" },
	{ 0x043C, "Scots Gaelic (Gaidhilge)" },
	{ 0x043D, "Yiddish" },
	{ 0x043E, "Malaysian" },
	{ 0x0801, "Arabic (Iraq)" },
	{ 0x0804, "Chinese (People's Republic of China)" },
	{ 0x0807, "German (Switzerland)" },
	{ 0x0809, "English (Britain)" },
	{ 0x080A, "Spanish (Mexican)" },
	{ 0x080C, "French (Belgium)" },
	{ 0x0810, "Italian (Switzerland)" },
	{ 0x0812, "Korean (Johab)" },
	{ 0x0813, "Dutch (Belgium)" },
	{ 0x0814, "Norwegian (Nynorsk)" },
	{ 0x0816, "Portuguese (Standard)" },
	{ 0x0818, "Romanian (Moldavia)" },
	{ 0x0819, "Russian (Moldavia)" },
	{ 0x081A, "Serbian" },
	{ 0x081D, "Swedish (Finland)" },
	{ 0x083C, "Gaelic (Irish)" },
	{ 0x0C01, "Arabic (Egypt)" },
	{ 0x0C04, "Chinese (Hong Kong SAR, China)" },
	{ 0x0C07, "German (Austria)" },
	{ 0x0C09, "English (Australia)" },
	{ 0x0C0A, "Spanish (Modern Sort)" },
	{ 0x0C0C, "French (Canada)" },
	{ 0x0C1A, "Serbian (Cyrillic)" },
	{ 0x1001, "Arabic (Libya)" },
	{ 0x1004, "Chinese (Singapore)" },
	{ 0x1007, "German (Luxembourg)" },
	{ 0x1009, "English (Canada)" },
	{ 0x100A, "Spanish (Guatemala)" },
	{ 0x100C, "French (Switzerland)" },
	{ 0x1401, "Arabic (Algeria)" },
	{ 0x1407, "German (Liechtenstein)" },
	{ 0x1409, "English (New Zealand)" },
	{ 0x140A, "Spanish (Costa Rica)" },
	{ 0x140C, "French (Luxembourg)" },
	{ 0x1801, "Arabic (Morocco)" },
	{ 0x1809, "English (Ireland)" },
	{ 0x180A, "Spanish (Panama)" },
	{ 0x1C01, "Arabic (Tunisia)" },
	{ 0x1C09, "English (South Africa)" },
	{ 0x1C0A, "Spanish (Dominican Republic)" },
	{ 0x2001, "Arabic (Oman)" },
	{ 0x2009, "English (Jamaica)" },
	{ 0x200A, "Spanish (Venezuela)" },
	{ 0x2401, "Arabic (Yemen)" },
	{ 0x2409, "English (Caribbean)" },
	{ 0x240A, "Spanish (Colombia)" },
	{ 0x2801, "Arabic (Syria)" },
	{ 0x2809, "English (Belize)" },
	{ 0x280A, "Spanish (Peru)" },
	{ 0x2C01, "Arabic (Jordan)" },
	{ 0x2C09, "English (Trinidad)" },
	{ 0x2C0A, "Spanish (Argentina)" },
	{ 0x3001, "Arabic (Lebanon)" },
	{ 0x300A, "Spanish (Ecuador)" },
	{ 0x3401, "Arabic (Kuwait)" },
	{ 0x340A, "Spanish (Chile)" },
	{ 0x3801, "Arabic (United Arab Emirates)" },
	{ 0x380A, "Spanish (Uruguay)" },
	{ 0x3C01, "Arabic (Bahrain)" },
	{ 0x3C0A, "Spanish (Paraguay)" },
	{ 0x4001, "Arabic (Qatar)" },
	{ 0x400A, "Spanish (Bolivia)" },
	{ 0x440A, "Spanish (El Salvador)" },
	{ 0x480A, "Spanish (Honduras)" },
	{ 0x4C0A, "Spanish (Nicaragua)" },
	{ 0x500A, "Spanish (Puerto Rico)" }
};

char* get_lcid_string(LCID lcid){
	uint i;
	for(i = 0; i < sizeof( lcid_strings )/sizeof( lcid_strings[0] ); i++){
		if( lcid_strings[i].lcid == lcid )
			return lcid_strings[i].lcid_string;
	}

	/* Returning "Unknown language" is not appropriate because the Language
	option in the HHP only has the language name when it is known. */
	return NULL;
}

char time_string[500];
char* time_format = "%c";

char* make_time_t_string( time_t ts ){
	struct tm* time = gmtime(&ts);
	if( !time ) return NULL;
	/* Another example of why POSIX sux ass - no possibility of dynamic allocation */
	return strftime( time_string, sizeof(time_string), time_format, time ) ? time_string : NULL;
}

/* FIXME: return something sensible
Converting to time_t is not good, because of capability differences,
perhaps use some code from WINE to convert to struct tm? */
char* make_FILETIME_string( FILETIME ts ){
	return NULL;
}
