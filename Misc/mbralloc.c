/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    mbralloc.c
 * Time-stamp: <2001-09-26 17:32:04 pagel>
 * Purpose: memory allocation and freeing
 * Authors:  Vincent Pagel and Alain Ruelle
 * Email :  mbrola@tcts.fpms.ac.be
 *
 * Copyright (c) 1995-2018 Faculte Polytechnique de Mons (TCTS lab)
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 09/04/98: Created
 *    Gather all allocation scheme here for people who wish to replace
 * it later with their own memory manager... at the moment nothing
 * really fancy  
 * 
 * FreeNull created by Alain Ruelle becomes MBR_free
 * 
 * 01/09/98: we don't have the catch/throw mechanism any more, so the
 * fatal_error handling becomes really brutal with exit(1)
 */

#include "common.h"
#include "vp_error.h"

void *MBR_malloc(size_t size)
/*
 * Check there's enough memory for the pointer
 */
{
	void *tmp=malloc(size);
  
	if (tmp==NULL)
    {
		fatal_message(ERROR_MEMORYOUT,"FATAL: out of memory\n");
		exit(1); /* Don't have any replacement solution */
    }
	return(tmp);
}

char *MBR_strdup( const char *str)
/* standard strdup would use standard malloc */
{
	char *newstr = (char *) MBR_malloc(strlen(str)+1); 
	strcpy(newstr,str);
	return(newstr);
}



