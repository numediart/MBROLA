/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    phone_export.cpp
 * Purpose: Phone objects, exported interface
 * Authors: Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <1999-06-11 17:30:19 pagel>
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
 * 11/06/99 : Created phone.h
 *  Function exported in the library in the case of a
 *  user defined Parser (the parser must return phonemes).
 */

#ifndef _PHONE_H
#define _PHONE_H

/* Dummy structure */
typedef void Phone;

Phone*  initSized_Phone(char* name, float length, int nb_pitch);
/*
 * Initialize a phoneme with its name and length in milliseconds
 * Indicate the planned number of pitch points ( added with appendf0)
 */

Phone*  init_Phone(char* name, float length);
/*
 * Initialize a phoneme with its name and length in milliseconds
 * 2 pitch points is the default (one at 0 one at 100)
 */

void  reset_Phone(Phone *ph);
/* Reset the pitch pattern list of a phoneme */

void  close_Phone(Phone *ph);
/* 
 * Release the name in the string 
 */

void  appendf0_Phone(Phone *ph, float pos, float f0);
/*
 * Append a pitch point to a phoneme ( position in % and f0 in Hertz )
 * resize the pitch point vector if too small
 */

#endif
