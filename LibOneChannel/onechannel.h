/*
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
 * File:    onechannel.h
 * Purpose: Diphone-based MBROLA speech synthesizer.
 *           One synthesis channel per database
 *
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 *
 * 10/01/97: Created -> library compilation mode 
 *           No main function but a Read/Write scheme
 */

#ifndef _ONECHANNEL_H
#define _ONECHANNEL_H

int DLL_EXPORT init_MBR(char *dbaname);
/* 
 * Reads the diphone database
 * 0 if ok, error code otherwise
 */ 

int DLL_EXPORT init_rename_MBR(char *dbaname,char* rename,char* clone);
/* 
 * Reads the diphone database
 * Rename and clone the list parsed from the parameter strings
 *
 * 0 if ok, error code otherwise
 */

void DLL_EXPORT close_MBR(void);
/*	Free all the allocated memory */

int DLL_EXPORT reset_MBR();
/* 
 * Reset the pho buffer with residual commands -> may be used as a kind of
 * "panic" flush when a sentence is interrupted
 * 0 means fail
 */

int DLL_EXPORT readtype_MBR(void *buffer_out, int nb_wanted, AudioType  sample_type);
/*
 * Read nb_wanted samples in an audio buffer
 * return the effective number of samples read
 * or the negative error code we catch
 *
 * The sample_type may be LIN16, LIN8, ULAW, ALAW
 */

int DLL_EXPORT read_MBR(void *buffer_out, int nb_wanted);
/*
 * Read nb_wanted samples in an audio buffer
 * return the effective number of samples read
 * or the negative error code we catch
 *
 * Kept for compatibility
 */

int DLL_EXPORT write_MBR(char *buffer_in);
/*
 * Write a string of phoneme in the input buffer
 * Return the number of chars actually written
 * 0 mean not enough space in the buffer
 */

int  DLL_EXPORT flush_MBR();
/*
 * Write a flush command in the stream (0 means fail). Used by client 
 * applications in case the flush symbol has been renamed
 */

int  DLL_EXPORT getDatabaseInfo_MBR(char *msg,int nb_wanted,int index);
/* Retrieve the ith info message, NULL means get the size */ 

void DLL_EXPORT setFreq_MBR(int freq);
/* Set the freq and voice ratio */

int  DLL_EXPORT getFreq_MBR();
/* Return the output frequency */

void DLL_EXPORT setNoError_MBR(int no_error);
/* Tolerance to missing diphones */

int DLL_EXPORT getNoError_MBR();
/* Spectral smoothing or not */

void DLL_EXPORT setVolumeRatio_MBR(float volume_ratio);
/* Overall volume */

float DLL_EXPORT getVolumeRatio_MBR();
/* Overall volume */

void DLL_EXPORT setParser_MBR(Parser* parser);
/* drop the current parser for a new one */

int DLL_EXPORT lastError_MBR();
/* Return the last error code */

int DLL_EXPORT lastErrorStr_MBR(char *buffer_err,int nb_wanted);
/* Return the last error message available */

void DLL_EXPORT resetError_MBR();
/* Clear the Mbrola error buffer */

int DLL_EXPORT getVersion_MBR(char *msg,int nb_wanted);
/* Return the release number, e.g. "2.05a"  */

#endif
