/*
 * File:    mbrola.h
 * Time-stamp: <00/04/11 13:13:16 pagel>
 * Purpose: Diphone-based MBROLA speech synthesizer.
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 *
 * FPMs-TCTS SOFTWARE LIBRARY
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
 */

#ifndef _MBROLA_H
#define _MBROLA_H

#include "diphone.h"
#include "database.h"
#include "parser.h"

#ifndef LIBRARY
#include "synth.h"
#endif

typedef struct 
{
	Database* diph_dba;   /* A synth engine is linked to a database */
	Parser* parser;       /* Phonemic command stream */
    
	/*
	 * prev_diph points to the previous diphone synthesis structure
	 * and cur_diph points to the current one. The reason is that to
	 * synthesize the previous diphone we need information on the next
	 * one. While progressing to the next diphone, prev_diph memory is
	 * resetted the pointers are swapped between cur and prev diphones
	 */
	DiphoneSynthesis *prev_diph, *cur_diph;

	/* Last_time_crumb balances slow time drifting in match_proso. time_crumb is
	 * the difference in samples between the length really synthesized and 
	 * theoretical one 
	 */
	int last_time_crumb;	
  
	float FirstPitch;     /* default first F0 Value (fetched in the database) */
	int32 audio_length;  /* File size, used for file formats other than RAW */

	int frame_number[NBRE_PM_MAX]; /* for match_prosody */
	int frame_pos[NBRE_PM_MAX];    /* frame position for match_prosody */

	int nb_begin;
	int nb_end; /* number of voiced frames at the begin and end the segment */

	bool saturation;    /* Saturation in ola_integer */
	float *ola_win;     /* OLA buffer                  */
	int16 *ola_integer; /* OLA buffer for file output  */

	float *weight;      /* Hanning weighting window */
	float volume_ratio; 	       /* 1.0 is default */
  
	/* 
	 * The following variables are part of the structure for library mode
	 * but could be local for standalone mode
	 */
	bool odd; /* flip-flop for reversing 1 out of 2 unvoiced OLA frame */
	int frame_counter;    /* frame being OLAdded  */
	int buffer_shift;		/* Shift between 2 Ola = available for output */
	int zero_padding;	   /* 0's between 2 Ola = available for output */
  
	bool smoothing;	      /* True if the smoothing algorithm is on */
	bool no_error;        /* True to ignore missing diphones */

	uint16 VoiceFreq;		   /* Freq of the audio output (vocal tract length) */
	float  VoiceRatio;    /* Freq ratio of the audio output */

#ifdef LIBRARY
	bool first_call;	/* True if it's the first call to Read_MBR */
	int eaten;	     /* Samples allready consumed in ola_integer */
#endif

} Mbrola;

/* Convenience macros */
#define diph_dba(mb)  mb->diph_dba
#define parser(mb)  mb->parser
#define prev_diph(mb)  mb->prev_diph
#define cur_diph(mb)  mb->cur_diph
#define last_time_crumb(mb)  mb->last_time_crumb
#define FirstPitch(mb)  mb->FirstPitch
#define audio_length(mb)  mb->audio_length
#define frame_number(mb)  mb->frame_number
#define frame_pos(mb)  mb->frame_pos
#define nb_begin(mb)  mb->nb_begin
#define nb_end(mb)  mb->nb_end
#define saturation(mb)  mb->saturation
#define ola_win(mb)  mb->ola_win
#define ola_integer(mb)  mb->ola_integer
#define weight(mb)  mb->weight
#define volume_ratio(mb)  mb->volume_ratio
#define odd(mb)  mb->odd
#define frame_counter(mb)  mb->frame_counter
#define buffer_shift(mb)  mb->buffer_shift
#define zero_padding(mb)  mb->zero_padding
#define smoothing(mb)  mb->smoothing
#define no_error(mb)  mb->no_error
#define VoiceRatio(pt) (pt->VoiceRatio)
#define VoiceFreq(pt) (pt->VoiceFreq)
#define first_call(pt) (pt->first_call)
#define eaten(pt) (pt->eaten)

void set_voicefreq_Mbrola(Mbrola* mb, uint16 OutFreq);
/* Change the Output Freq and VoiceRatio to change the vocal tract   */

uint16 get_voicefreq_Mbrola(Mbrola* mb);
/* Get output Frequency */

void set_smoothing_Mbrola(Mbrola* mb, bool smoothing);
/* Spectral smoothing or not */

bool get_smoothing_Mbrola(Mbrola* mb);
/* Spectral smoothing or not */

void set_no_error_Mbrola(Mbrola* mb, bool no_error);
/* Tolerance to missing diphones */

bool get_no_error_Mbrola(Mbrola* mb);
/* Spectral smoothing or not */

void set_volume_ratio_Mbrola(Mbrola* mb, float volume_ratio);
/* Overall volume */

float get_volume_ratio_Mbrola(Mbrola* mb);
/* Overall volume */

void set_parser_Mbrola(Mbrola* mb, Parser* parser);
/* drop the current parser for a new one */

Mbrola* init_Mbrola(Database* dba);
/* 
 * Connect the database to the synthesis engine, then initialize internal 
 * variables. Connect the phonemic command stream later with set_parser_Mbrola
 */

void close_Mbrola(Mbrola* mb);
/* close related features and free the memory ! */

bool reset_Mbrola(Mbrola* mb);
/* 
 * Gives initial values to current_diphone (not synthesized anyway)
 * -> it will give a first value for prev_diph when we make the first
 *    NextDiphone call so that cur_diph= _-FirstPhon with lenght1=0
 *    and prev_diph= _-_ with length2=0
 *
 * return False in case of error
 */

StatePhone NextDiphone(Mbrola* mb);
/*
 * Reads a phone from the phonetic command buffer and prepares the next
 * diphone to synthesize ( prev_diph )
 * Return value may be: PHO_EOF, PHO_FLUSH, PHO_OK, PHO_ERROR
 */

bool MatchProsody(Mbrola* mb);
/*
 * Selects Duplication or Elimination for each analysis OLA frames of
 * the diphone we must synthesize (prev_diph). Selected frames must fit
 * with wanted pitch pattern and phonemes duration of prev_diph
 *
 * Return False in case of error
 */

void Concat(Mbrola* mb);
/*
 * This is a unique feature of MBROLA.
 * Smoothes diphones around their concatenation point by making the left
 * part fade into the right one and conversely. This is possible because
 * MBROLA frames have the same length everywhere plus phase trick.
 *
 * output : nb_begin, nb_end -> number of stable voiced frames to be used
 * for interpolation at the end of Leftphone(prev_diph) and the beginning
 * of RightPhone(prev_diph).
 */

void OverLapAdd(Mbrola* mb, int frame);
/*
 *  OLA routine
 */

#ifdef LIBRARY

/* LIBRARY mode: synthesis driven by the output */

int readtype_Mbrola(Mbrola* mb, void *buffer_out, int nb_wanted, AudioType sample_type);
/*
 * Reads nb_wanted samples in an audio buffer
 * Returns the effective number of samples read
 */

#else

/* STANDALONE MODE: Synthesis driven by the input */

StatePhone Synthesis(Mbrola* mb);
/*
 * Main loop: performs MBROLA synthesis of all diphones
 * Returns a value indicating the reasons of the break
 * (a flush request, a end of file, end of phone sequence)
 */

#endif


#endif
