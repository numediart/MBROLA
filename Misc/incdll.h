/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    incdll.h
 * Purpose: symbols needed outside of the mbrola sources
 *          namely to compile the wrapper DLL
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
 */

#ifndef INCDLL_H
#define INCDLL_H

/* 
 * Type of samples we can output with read_MBR
 */

typedef enum {
	LIN16=0,     /* same as intern computation format: 16 bits linear  */
	LIN8,        /* unsigned linear 8 bits, worse than telephone        */
	ULAW,        /* MU law -> 8bits, telephone. Roughly equ. to 12bits */
	ALAW         /* A law  -> 8bits, equivallent to mulaw              */
} AudioType;

#endif
