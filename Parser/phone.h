/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    phone.cpp
 * Purpose: Phone objects
 * Authors: Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <2000-03-28 17:50:14 pagel>
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
 * 23/06/98 : Created from diphone.cpp
 *  The functions are exported to the DLL in the case of a
 *  user defined Parser (the parser must return phonemes).
 */

#ifndef _PHONE_H
#define _PHONE_H

#include "common.h"

/*
 * STRUCTURES representing phones and diphone sequences to synthesize
 */

/* Pitch pattern point attached to a Phoneme */
typedef struct
{
	float pos;		        /* relative position within phone in milliseconds */
	float freq;		        /* frequency (Hz)*/
} PitchPatternPoint;

#define pos_Pitch(X) X->pos
#define freq_Pitch(X) X->freq

/* A Phoneme and its pitch points */
typedef struct
{
	char *name;       	              /* Name of the phone       */
	float length;	                 /* phoneme length in ms    */
	int  NPitchPatternPoints;        /* Nbr of pattern points   */
	int  pp_available;               /* number of allocatables pitch points  */
	PitchPatternPoint* PitchPattern;
	/* PitchPattern[0] gives F0 at 0% of the duration of a phone,
	   and the last pattern point (PitchPattern[NPitchPatternPoints-1])
	   gives F0 at 100% ( reserve 2 slots for 0% and 100% during interpolation )	 */
} Phone;

/* Convenient macro to access Phone structure */
#define tail_PitchPattern(X) (&(X->PitchPattern[X->NPitchPatternPoints-1]))
#define head_PitchPattern(X) (&(X->PitchPattern[0]))
#define val_PitchPattern(X,i) (&(X->PitchPattern[i]))
#define length_Phone(X) (X->length)
#define name_Phone(X) (X->name)
#define NPitchPatternPoints(X) (X->NPitchPatternPoints)
#define pp_available(X) (X->pp_available)
#define PitchPattern(X) (X->PitchPattern)

Phone* DLL_EXPORT initSized_Phone(char* name, float length, int nb_pitch);
/*
 * Initialize a phoneme with its name and length in milliseconds
 * Indicate the planned number of pitch points ( added with appendf0)
 */

Phone* DLL_EXPORT init_Phone(char* name, float length);
/*
 * Initialize a phoneme with its name and length in milliseconds
 * 2 pitch points is the default (one at 0 one at 100)
 */

void DLL_EXPORT reset_Phone(Phone *ph);
/* Reset the pitch pattern list of a phoneme */

void DLL_EXPORT close_Phone(Phone *ph);
/* 
 * Release the name in the string 
 */

void DLL_EXPORT appendf0_Phone(Phone *ph, float pos, float f0);
/*
 * Append a pitch point to a phoneme ( position in % and f0 in Hertz )
 * resize the pitch point vector if too small
 */

void applyRatio_Phone(Phone* ph, float ratio);
/* 
 * length and freq modified if the vocal tract length is not 1.0 
 * internal use only
 */

#endif
