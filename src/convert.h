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

#ifndef _CONVERT_H_
#define _CONVERT_H_

#include <time.h>
#include "common.h"

char* get_lcid_string(LCID i);
char* make_time_t_string(time_t i);
char* make_FILETIME_string(FILETIME i);
void print_with_entity_refs( FILE* f, char* str );

#endif /* _CONVERT_H_ */
