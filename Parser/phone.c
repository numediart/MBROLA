/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    phone.c
 * Purpose: Phone objects
 * Authors: Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <2000-03-28 17:57:46 pagel>
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
 * 23/06/98 : Created from diphone.c
 *  The functions are exported to the DLL in the case of a
 *  user defined Parser (the parser must return phonemes).
 *
 * 28/08/98 : added applyRatio_Phone to modify length and freq according
 *    to a vocal tract ratio. It ends here because the Vocal Tract ratio
 *    is not the business of the Parser (so the variable does leave the
 *    Engine section).
 *
 * 15/09/98 : appendf0_Phone now enlarge the pitch point table if it's
 *    too small (no more "fatal_error").
 */

#include "phone.h"

Phone* DLL_EXPORT initSized_Phone(char* name, float length, int nb_pitch)
/*
 * Initialize a phoneme with its name and length in milliseconds
 * Indicate the planned number of pitch points ( added with appendf0)
 */
{
	Phone* self= (Phone*) MBR_malloc(sizeof(Phone));
	name_Phone(self)=MBR_strdup(name);
	length_Phone(self)=length;
	reset_Phone(self);

	/* allocate the pitch slots */
	pp_available(self)= nb_pitch;
	self->PitchPattern= (PitchPatternPoint*) 
		MBR_malloc( sizeof(PitchPatternPoint) * pp_available(self));
	return(self);
}

Phone* DLL_EXPORT init_Phone(char* name, float length)
/*
 * Initialize a phoneme with its name and length in milliseconds
 * 2 pitch points is the default (one at 0 one at 100)
 */
{ return initSized_Phone(name,length,2);  }

void DLL_EXPORT reset_Phone(Phone *ph)
/* Reset the pitch pattern list of a phoneme */
{
	ph->NPitchPatternPoints=0;
}

void DLL_EXPORT close_Phone(Phone *ph)
/* 
 * Release the name in the string 
 */
{
	if (name_Phone(ph))
		MBR_free(name_Phone(ph));

	MBR_free( PitchPattern(ph) );
	MBR_free(ph);
}

void DLL_EXPORT appendf0_Phone(Phone *ph, float pos, float f0)
/*
 * Append a pitch point to a phoneme ( position in % and f0 in Hertz )
 * resize the pitch point vector if too small
 */
{
	int index_f0= NPitchPatternPoints(ph);

	/* Enlarge the pitch point vector ? */
	if (index_f0 == (pp_available(ph)-1))
    {
		pp_available(ph)+=2;
		PitchPattern(ph)= (PitchPatternPoint*) MBR_realloc( PitchPattern(ph), 
															sizeof(PitchPatternPoint) * pp_available(ph));
    }
  
	pos_Pitch( val_PitchPattern(ph,index_f0))= pos/100.0f * length_Phone(ph);
	freq_Pitch( val_PitchPattern(ph,index_f0))= f0;
	NPitchPatternPoints(ph)++;
}

void applyRatio_Phone(Phone* ph, float ratio)
/* length and freq modified if the vocal tract length is not 1.0 */
{
	int i;

	/* need to change anything ? */
	if (ratio==1.0)
		return;

	length_Phone(ph)*= ratio;
	for(i=0; i< ph->NPitchPatternPoints; i++)
    {
		ph->PitchPattern[i].pos *= ratio;
		ph->PitchPattern[i].freq /= ratio;
    }
}
