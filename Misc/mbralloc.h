/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    mbralloc.h
 * Time-stamp: <2000-03-28 17:53:02 pagel>
 * Purpose: memory allocation and freeing
 * Author:  Vincent Pagel and Alain Ruelle
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
 *    Gather all allocation scheme here for people who wish to replace it
 *  later with their own memory manager
 *  Alain Ruelle's FreeNull becomes MBR_free
 */
#ifndef _MBRALLOC_H
#define _MBRALLOC_H

#include <stdio.h>
#include <stdlib.h>

#define MBR_free(X)		{free(X);X=NULL;}
/* free a memory block and set the pointer to NULL */ 

#define MBR_realloc(X,Y)		realloc(X,Y)
/* dummy reallocation for the moment */ 

void *MBR_malloc(size_t size);
/*
 * Check there's enough memory for the pointer
 */

char *MBR_strdup( const char *str);
/* standard strdup would use standard malloc */

#endif
