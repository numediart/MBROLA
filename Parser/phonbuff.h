/* FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    phonbuff.c
 * Purpose: Table of phonemes to implement a simple .pho parser
 *          Buffer of phonemes for pitch interpolation
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <1998-10-21 16:14:51 pagel>
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
 * 11/06/98 : Created from readpho.c
 *            Warning -> this is a sample program of what a parser could
 *            be, feel free to implement your own parser (as long as it 
 *            follows the requirements of parser.h)
 *
 * 20/10/98 : initialize flush from constructor
 */

#ifndef _PHONEBUFF_H
#define _PHONEBUFF_H

#include "diphone.h"
#include "input.h"
#include "parser.h"

#define MAXNPHONESINONESHOT 250    /* Max nbr of phonemes without F0 pattern*/

/* A phonetic command buffer and its pitch points */
typedef struct
{
	Input* input;		/* Polymorphic input stream */
  
	Phone* Buff[MAXNPHONESINONESHOT];/* Phonetic command buffer  */
	int NPhones;     /* Nbr of phones in the phonetic command buffer  */
	int CurPhone;   /* Index of current phone in the command buffer  */
	StatePhone state_pho;   /* State of the last phoneme serie: EOF FLUSH OK */
	bool Closed;	   /* True if the sequence is closed by a pitch point */

	/*
	 * Silence is a special phoneme used for initialization (the first diphone
	 * in the stream is always SILENCE-SILENCE. Also used for termination
	 */
	char *default_phon;
	float default_pitch; /* first pitch point of the sequence */

	char *comment_symbol; /* user defined escape char */
	char *flush_symbol;   /* user defined flush command */

	float TimeRatio;  /* Ratio for the durations of the phones */
	float FreqRatio;  /* Ratio for the pitch applied to the phones */
} PhoneBuff;

/* Convenient macro to access Phonetable */
#define input(X) (X->input)
#define CurPho(X) (X->Buff[X->CurPhone])
#define NPhones(X) X->NPhones
#define CurPhone(X) X->CurPhone
#define Buff(X) X->Buff
#define val_PhoneBuff(pt,i) (pt->Buff[i])
#define state_pho(pt) (pt->state_pho)
#define Closed(pt) (pt->Closed)
#define default_phon(pt) (pt->default_phon)
#define default_pitch(pt) (pt->default_pitch)

#define comment_symbol(pt) (pt->comment_symbol)
#define flush_symbol(pt) (pt->flush_symbol)

#define TimeRatio(pt) (pt->TimeRatio)
#define FreqRatio(pt) (pt->FreqRatio)

/* 
 * Last phone of the list
 */
#define tail_PhoneBuff(pt) (val_PhoneBuff(pt,NPhones(pt)))

/* 
 * First phone of the list
 */
#define head_PhoneBuff(pt) (val_PhoneBuff(pt,0))

PhoneBuff* init_PhoneBuff(Input* my_input, char* default_phon,float default_pitch, float time_ratio, float freq_ratio,char* comment, char* flush);
/* 
 * Constructor, needs a phoneme and pitch for default allocation (begin
 * and end of synthesis)
 */

void close_PhoneBuff(PhoneBuff *pt);
/* free allocated strings in the phonetable */

void reset_PhoneBuff(PhoneBuff *pt);
/* Before a synthesis sequence initialize the loop with a silence */

StatePhone next_PhoneBuff(PhoneBuff *pt,Phone** ph);
/*
 * Reads a phone from the phonetic command buffer and prepares the next
 * diphone to synthesize ( prev_diph )
 * Return value may be: PHO_EOF,PHO_FLUSH,PHO_OK, PHO_ERROR
 *
 * NB : Uses only phones from 1 to ... NPhones-1 in the buffer.
 * Phone 0 = memory from previous buffer.
 */

void init_FlushSymbol(PhoneBuff *pt, char *flush);
/* 
 * Build a new sscanf target to spot the flush symbol
 */

void init_CommentSymbol(PhoneBuff *pt, char *comment);
/* 
 * Build a new sscanf target to spot the comment symbol
 */

void init_SilenceSymbol(PhoneBuff *pt, char *silence);
/* 
 * Build a new sscanf target to spot the comment symbol
 */

#endif
