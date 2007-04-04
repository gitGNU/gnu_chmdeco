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

#ifndef _WINDOWS_H_
#define _WINDOWS_H_

extern DWORD num_windows;

bool open_windows( void );
bool print_windows_entry(FILE* hhp);
void close_windows( void );

#endif /* _WINDOWS_H_ */
