/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
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
 * File:  multichannel.h
 * Purpose: multichannel Mbrola synthesis
 * Authors: Pagel Vincent
 * Email : mbrola@tcts.fpms.ac.be
 *
 * 22/06/98: Created. Replace old library.c
 *           One should either use multichannel or onechannel front end
 *           depending on end-user or telecom applications
 */

#ifndef _MULTICHANNEL_H
#define _MULTICHANNEL_H

#include "database.h"
#include "mbrola.h"
#include "parser.h"

Database* DLL_EXPORT init_DatabaseMBR2(char* dbaname, char* rename, char* clone);
/* 
 * Give the name of the file containing the database, and parameters to 
 * rename of clone phoneme names
 *
 * NULL on rename or clone means no modification to the database 
 */

Database* DLL_EXPORT copyconstructor_DatabaseMBR2(Database* dba);
/* Creates a copy of a diphone database so that many synthesis engine 
 * can use the same database at the same time (duplicate the file handler)
 *
 * Highly recommended with multichannel mbrola, unless you can guaranty
 * mutually exclusive access to the getdiphone function
 */

void DLL_EXPORT close_DatabaseMBR2(Database* dba);
/* Release the memory */

void DLL_EXPORT close_ParserMBR2(Parser* pars);
/* 
 * Release the memory of the polymorphic type
 */

Mbrola* DLL_EXPORT init_MBR2(Database* db, Parser* parse);
/* Kick start the engine. Returning NULL means error */

void DLL_EXPORT close_MBR2(Mbrola* mb);
/*	Free everything */

int DLL_EXPORT reset_MBR2(Mbrola* mb);
/* 
 * Reset the pho buffer with residual commands -> used as a kind of
 * "panic" flush when a sentence is interrupted either with the stop
 * button, or in case of error
 * Return false in case of failure
 */

int DLL_EXPORT readtype_MBR2(Mbrola* mb, void *buffer_out, int nb_wanted, AudioType sample_type);
/*
 * Reads nb_wanted samples in an audio buffer
 * Returns the effective number of samples read
 */

int DLL_EXPORT getDatabaseInfo_MBR2(Mbrola* mb,char *msg,int nb_wanted,int index);
/* Retrieve the ith info message, NULL means get the size  */ 

void DLL_EXPORT setFreq_MBR2(Mbrola* mb,int freq);
/* Set the freq and voice ratio */

int  DLL_EXPORT getFreq_MBR2(Mbrola* mb);
/* Return the output frequency */

void DLL_EXPORT setSmoothing_MBR2(Mbrola* mb, int smoothing);
/* Spectral smoothing or not */

int DLL_EXPORT getSmoothing_MBR2(Mbrola* mb);
/* Spectral smoothing or not */

void DLL_EXPORT setNoError_MBR2(Mbrola* mb, int no_error);
/* Tolerance to missing diphones */

int DLL_EXPORT get_no_error_MBR2(Mbrola* mb);
/* Spectral smoothing or not */

void DLL_EXPORT set_volume_ratio_MBR2(Mbrola* mb, float volume_ratio);
/* Overall volume */

float DLL_EXPORT get_volume_ratio_MBR2(Mbrola* mb);
/* Overall volume */

void DLL_EXPORT set_parser_MBR2(Mbrola* mb, Parser* parser);
/* drop the current parser for a new one */

int DLL_EXPORT lastError_MBR2();
/* Return the last error code */

int DLL_EXPORT lastErrorStr_MBR2(char *buffer_err,int nb_wanted);
/* Return the last error message available */

void DLL_EXPORT reset_error_MBR2();
/* Clear the Mbrola error buffer */

int DLL_EXPORT getVersion_MBR2(char *msg,int nb_wanted);
/* Return the release number, e.g. "2.05a"  */

#endif
