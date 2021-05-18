/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    common.c
 * Time-stamp: <00/03/29 14:36:29 pagel>
 * Purpose: common defines and utilities
 * Author:  Vincent Pagel
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
 * 29/02/96 : Created. Put here all little usefull functions
 * and symbols common to the different sources.
 *
 */

#include "common.h"

#if defined(TARGET_OS_VMS) || defined(TARGET_OS_BEOS)
void swab( const char *from, char *to, int nbytes)
/* A quick definition of SWAB for VAX-VMS  */
{
	int i;
	char tmp;
    
	for (i=0; i<nbytes; i+=2)
    {
		tmp=from[i];
		to[i]=from[i+1];
		to[i+1]=tmp;
    }
}

#endif
