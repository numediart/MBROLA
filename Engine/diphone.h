/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    diphone.c
 * Time-stamp: <2000-04-07 16:55:15 pagel>
 * Purpose: Phone and diphone objects
 * Authors: Vincent Pagel
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
 * 15/06/98 : Created. Phones, Diphones, ...
 * 09/09/98 : reset_DiphoneSynthesis function
 * 24/03/00 : no more limits on period size -> pass frame size during construction
 */

#ifndef _DIPHONE_H
#define _DIPHONE_H

#include "phone.h"
#include "diphone_info.h"

/*
 * PITCH MARKED DIPHONE DATABASE, CONSTANTS
 */
#define NBRE_PM_MAX	2000  /* Max nbr of frames in a synth. segment*/

/*
 * STRUCTURES representing diphone sequences to synthesize
 */

/* 
 * A DiphoneSynthesis is a Diphone equiped with information necessary to 
 * synthesize it
 */
typedef struct
{
	/* A Diphone is made of 2 phonemes */
	Phone *LeftPhone; /* First phoneme  */
	Phone *RightPhone;/* Second phoneme */

	int   Length1; /* Length of first half-phoneme in samples */
	int   Length2; /* Length of second half-phoneme in samples */

	DiphoneInfo* Descriptor;	 /* Descriptor in the diphone database      */
	uint8 *p_pmrk;		/* Point to the beginning of the pm 1..N interval */
	uint8 p_pmrk_offset;  /* offset in the 4 bit compressed structure */
  
	int16 *smoothw;    /* Difference vector between 2 ola frames (2 mbr_period) */
	bool smooth;		   /* True if Smoothw has a value */

	int16* buffer;     /* To read or uncompress audio data */
	bool buffer_alloced; /* True if pointer above was malloc'ed */
   
	uint8 *real_frame; /*  for skiping V - NV transition */
	uint8 tot_frame;   /* physical number of frames of the diphone */
	int nb_pm;			/* Number of pitch markers to synthesize */
} DiphoneSynthesis;

/*
 * Convenient macros to access Diphone_synth structures
 */
#define halfseg_diphone(X) X->Descriptor->halfseg
#define nb_frame_diphone(X) X->Descriptor->nb_frame
#define pos_wave_diphone(X) X->Descriptor->pos_wave

#define left_diphone(MB,X)  auxiliary_tab_val( diphone_table(diph_dba(MB)) , X->Descriptor->left)
#define right_diphone(MB,X) auxiliary_tab_val( diphone_table(diph_dba(MB)) , X->Descriptor->right)

#define Descriptor(X) (X->Descriptor)
#define Length1(X) X->Length1
#define Length2(X) X->Length2
#define LeftPhone(X) (X->LeftPhone)
#define RightPhone(X) (X->RightPhone)

#define nb_pm(X) X->nb_pm
#define smoothw(X) X->smoothw
#define smooth(X) X->smooth
#define buffer(X) X->buffer
#define buffer_alloced(X) X->buffer_alloced
#define real_frame(X) X->real_frame
#define physical_frame_type(X) X->physical_frame_type
#define tot_frame(X) X->tot_frame

/* At the moment it's a macro */
#define pmrk_DiphoneSynthesis(DP,INDEX) ((DP->p_pmrk[ ( (INDEX-1)+DP->p_pmrk_offset)/ 4 ] >> (  2*( ((INDEX-1) + DP->p_pmrk_offset)%4))) & 0x3)

	DiphoneSynthesis* init_DiphoneSynthesis(int mbr_period, int max_pm, int max_sample);
/* Alloc memory, working and audio buffers for synthesis */

void reset_DiphoneSynthesis(DiphoneSynthesis* ds);
/*
 * Forget the diphone in progress
 */

void close_DiphoneSynthesis(DiphoneSynthesis* ds);
/* Release memory and phone */

int GetPitchPeriod(DiphoneSynthesis *dp, int cur_sample,int Freq);
/*
 * Returns the pitch period (in samples) at position cur_sample 
 * of dp by linear interpolation between pitch pattern points.
 */

#endif

