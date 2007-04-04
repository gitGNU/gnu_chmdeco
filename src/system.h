/*
chmdeco -- extract files from ITS/CHM files and decompile CHM files
Copyright 2003-2007 Paul Wise

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

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "common.h"

enum SYSTEM_CODE {
	Contents_file_CODE = 0,
	Index_file_CODE,
	Default_topic_CODE,
	Title_CODE,
	Language_DBCS_FTS_KLinks_ALinks_FILETIME_CODE,
	Default_Window_CODE,
	Compiled_file_CODE,
	Binary_Index_CODE,
	ABBREVIATIONS_N_EXPLANATIONS_CODE,
	COMPILED_BY_CODE,
	time_t_STAMP_CODE,
	Binary_TOC_CODE,
	NUM_INFORMATION_TYPES_CODE,
	IDXHDR_FILE_CODE,
	Custom_tabs_CODE,
	INFORMATION_TYPE_CHECKSUM_CODE,
	Default_Font_CODE,
	MAX_CODE
};

struct system_entry{
	int offset;
	WORD length;
};

extern struct system_entry system_entries[MAX_CODE];
extern DWORD system_version;

bool open_system( void );
BYTE* get_system(WORD);
bool get_system_DWORD(WORD code, DWORD* d);
bool get_system_to(WORD code, BYTE* buf, WORD length);
void close_system( void );

#endif /* _SYSTEM_H_ */
