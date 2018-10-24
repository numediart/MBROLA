/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    diphoneinfo.c
 * Purpose: diphone descriptor
 * Authors:  Vincent Pagel & Alain Ruelle
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
 * 09/04/98 : Created. A diphone descriptor intended for a hash_table
 *
 * 09/09/98 : DiphoneInfo now includes memory allocation and dereference
 *
 * 03/03/00 : Use PhonemeCode instead of PhonemeName to get a flat structure
 *            no more memory memory allocation needed
 */

#include "diphone_info.h"
#include "mbralloc.h"

int32 hash_DiphoneInfo(const char* left_string, const char* right_string)
/* 
 * Hashing function for the research of the diphone name in diphone_table
 */
{
	int32 mult;
	int i,shift;
  
	/*  nb_empty=433 nb_collision=309
	 * 	504 alone and rest is 742 / 309 = 2.401294 for one hash code 
	 *
	 * On 1000 phoneme (Levai.pho) average 1.546953 comparison to find the key
	 * Systematic search for diphones gives average 1.5955 path
	 *
	 * Adding extra 25% space in the hash table leads to 1.427 comparisons
	 * to find the key
	 */
	shift=0;
	mult=0;
	for(i=0;left_string[i]!=0;i++)
	{
		mult+= left_string[i] << shift;
		shift=(shift+8)%32;
	}
	
	for(i=0;right_string[i]!=0;i++)
	{
		mult+= right_string[i] << shift ;
		shift=(shift+8)%32;
	} 
	return(mult);
}
