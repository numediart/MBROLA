/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    audio_file.h
 * Time-stamp: <2000-03-28 17:53:49 pagel>
 * Purpose: audio files
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
 */

#ifndef _AUDIO_H
#define _AUDIO_H

#include "common.h"

/*
 * Audio file format
 */
typedef enum {
  RAW_FORMAT=0,     /* same as intern computation format: 16 bits linear  */
  WAV_FORMAT ,
  AU_FORMAT  , 
  AIF_FORMAT ,
  AIFF_FORMAT 
} WaveType;

int write_int16s(int16 *buffer,int count,FILE *file);
/* Write a buffer of int16 */

void write_header(WaveType file_format, int32 audio_length, uint16 samp_rate, FILE *output_file);
/* Write the header corresponding to the output audio format */

WaveType find_file_format(char *name);
/* Find the file format corresponding to the name's extension  
 * raw=none wav=RIFF au=Sun Audio aif or aiff=Macintosh
 */

/* 
 * Sample type conversion routines for read_MBR
 */
#ifdef LIBRARY

void* zero_convert(void* buffer_out, int nb_move, AudioType sample_type);
/*
 * Output zeros in a buffer according to the sample_type
 * Return the next position after the end of the buffer
 *
 * Returning NULL means fatal error
 */

void* move_convert(void* buffer_out,int16* buffer_in,int nb_move, AudioType sample_type);
/* 
 * Move audio samples and convert them at the same time 
 * Return the shifted pointer in buffer_out
 *
 * linear 16bits to linear16 :-) simply move 
 * linear 16bits to linear8 
 * linear 16bits to mulaw 
 * linear 16bits to alaw 
 *
 * Returning NULL means fatal error
 */

#endif

#endif
