/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    diphoneinfo.c
 * Time-stamp: <00/03/29 23:38:54 pagel>
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

#ifndef DIPHONE_INFO_H
#define DIPHONE_INFO_H

#include "common.h"

/*
 * Structure of the diphone database (as stored in memory)
 */
typedef struct
{
	/* Name of the diphone     */
	PhonemeCode left;
	PhonemeCode right;	

	int32 pos_wave;	/* position in SPEECH_FILE */
	int16 halfseg;	/* position of center of diphone */
	int32 pos_pm;		/* index in PITCHMARK_FILE */
	uint8 nb_frame;    	/* Number of pitch markers */
} DiphoneInfo;

/* Convenience macros */
#define left(diphoneinfo) (diphoneinfo).left
#define right(diphoneinfo) (diphoneinfo).right
#define pos_wave(diphoneinfo) (diphoneinfo).pos_wave
#define halfseg(diphoneinfo) (diphoneinfo).halfseg
#define pos_pm(diphoneinfo) (diphoneinfo).pos_pm
#define nb_frame(diphoneinfo) (diphoneinfo).nb_frame

int32 hash_DiphoneInfo(const char* left, const char* right);
/* 
 * Hashing function for the research of the diphone name in diphone_table
 */

#endif
