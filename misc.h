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

#ifndef _MISC_H_
#define _MISC_H_

#include "common.h"

#define FTS_STOP_LIST_FILE_NAME "fts-stop-list-file.stp"
extern bool stp_success;
void recreate_fts_stop_list_file( void );

#define ALIAS_FILE_NAME "alias.ali"
#define MAP_FILE_NAME "map.h"
extern bool alias_map_success;
void recreate_alias_map( void );

#define Sample_list_file "Sample-list-file.hhs"
extern bool samples_success;
extern char Sample_Staging_Path[8];
void recreate_samples( void );

void recreate_html_from_fts( void );


#endif /* _MISC_H_ */
