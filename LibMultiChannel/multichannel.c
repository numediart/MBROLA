/*
 * FPMs-TCTS SOFTWARE LIBRARY
 * Time-stamp: <2000-03-28 18:03:07 pagel>
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
 * File:  multichannel.c
 * Purpose: instanciation of multiple channel from one dba
 * Authors: Pagel Vincent
 * Email : mbrola@tcts.fpms.ac.be
 *
 * 22/06/98: Created
 * 23/08/98: Correction in close_MBR2 (bug detected by Susan Eisman)
 *           Change function names for consistence
 * 
 * 03/09/98: close_MBR2 don't have to close the database and parser...
 *           export the kit init/close_Database for the DLL client
 *
 * 09/09/98: 3.01d -> reset_MBR2 now returns a success value
 *                    first_call(mb) goes back to where it belongs (Mbrola)
 *                    and correct a bug with init/reset_MBR2
 * 20/10/98: 3.01g -> pass flush symbol. Avoid the variable "rename" due to
 *           exisiting functions in libraries
 */

#include "common.h"
#include "diphone.h"
#include "audio.h"
#include "mbrola.h"
#include "database.h"
#include "input_fifo.h"
#include "input_file.h"
#include "incdll.h"

Database* DLL_EXPORT init_DatabaseMBR2(char* dbaname, char* rename_string, char* clone_string)
/* 
 * Give the name of the file containing the database, and parameters to 
 * rename of clone phoneme names
 *
 * NULL on rename or clone means no modification to the database 
 */
{
	ZStringList* rename_list=NULL;     /* phoneme renaming */
	ZStringList* clone_list=NULL;      /* phoneme cloning */
  
	if (rename_string)
    {
		rename_list= init_ZStringList();
		parse_ZStringList(rename_list, rename_string, False);
    }
	if (clone_string)
    {
		clone_list= init_ZStringList();
		parse_ZStringList(clone_list, clone_string, True);
    }
	return init_rename_Database(dbaname, rename_list, clone_list);
}

Database* DLL_EXPORT copyconstructor_DatabaseMBR2(Database* dba)
/* Creates a copy of a diphone database so that many synthesis engine 
 * can use the same database at the same time (duplicate the file handler)
 *
 * Highly recommended with multichannel mbrola, unless you can guaranty
 * mutually exclusive access to the getdiphone function
 */
{
	return copyconstructor_Database(dba);
}

void DLL_EXPORT close_DatabaseMBR2(Database* dba)
/*
 * Release the memory of the polymorphic type
 */
{
	dba->close_Database(dba); /* virtual destructor */
}


void DLL_EXPORT close_ParserMBR2(Parser* pars)
/* 
 * Release the memory of the polymorphic type
 */
{
	pars->close_Parser(pars);
}


Mbrola* DLL_EXPORT init_MBR2(Database* db, Parser* parse)
/* 
 * Kick start the engine. Returning NULL means error
 */
{
	Mbrola* mb;
  
	mb= init_Mbrola(db);

	if (mb)
		set_parser_Mbrola(mb, parse);

	return(mb);
}

void DLL_EXPORT close_MBR2(Mbrola* mb)
/*	Free everything */
{
	close_Mbrola(mb);		                /* Close the engine */
}

int DLL_EXPORT reset_MBR2(Mbrola* mb)
/* 
 * Reset the pho buffer with residual commands -> used as a kind of
 * "panic" flush when a sentence is interrupted either with the stop
 * button, or in case of error
 * Return false in case of failure
 */
{
	if (!reset_Mbrola(mb))
		return False;
  
	parser(mb)->reset_Parser(parser(mb));
  
	return True;
}

int DLL_EXPORT readtype_MBR2(Mbrola* mb, void *buffer_out, int nb_wanted, AudioType sample_type)
/*
 * Reads nb_wanted samples in an audio buffer
 * Returns the effective number of samples read
 */
{ return readtype_Mbrola(mb, buffer_out, nb_wanted, sample_type); }

int DLL_EXPORT getDatabaseInfo_MBR2(Mbrola* mb,char *msg,int nb_wanted,int index)
/* Retrieve the ith info message, NULL means get the size  */ 
{ return(getDatabaseInfo(diph_dba(mb), msg, nb_wanted, index)); }

void DLL_EXPORT setFreq_MBR2(Mbrola* mb,int freq)
/* Set the freq and voice ratio */
{ set_voicefreq_Mbrola(mb,freq); }

int  DLL_EXPORT getFreq_MBR2(Mbrola* mb)
/* Return the output frequency */
{ return(VoiceFreq(mb)); }

void DLL_EXPORT setNoError_MBR2(Mbrola* mb, int no_error)
/* Tolerance to missing diphones */
{ set_no_error_Mbrola(mb, no_error); }

int DLL_EXPORT getNoError_MBR2(Mbrola* mb)
/* Spectral smoothing or not */
{ return get_no_error_Mbrola(mb); }

void DLL_EXPORT setVolumeRatio_MBR2(Mbrola* mb, float volume_ratio)
/* Overall volume */
{ set_volume_ratio_Mbrola(mb, volume_ratio); }

float DLL_EXPORT getVolumeRatio_MBR2(Mbrola* mb)
/* Overall volume */
{ return get_volume_ratio_Mbrola(mb); }

void DLL_EXPORT setParser_MBR2(Mbrola* mb, Parser* parser)
/* drop the current parser for a new one */
{ set_parser_Mbrola(mb, parser); }

int DLL_EXPORT lastError_MBR2()
/* Return the last error code */
{  return lasterr_code; }

int DLL_EXPORT lastErrorStr_MBR2(char *buffer_err,int nb_wanted)
/* Return the last error message available */
{
	int length=strlen(errbuffer)+1;
  
	if (length<nb_wanted) 
		nb_wanted=length;
  
	memcpy(buffer_err,errbuffer,nb_wanted-1);
	buffer_err[nb_wanted-1]=0;
  
	return nb_wanted;
}

void DLL_EXPORT resetError_MBR2()
/* Clear the Mbrola error buffer */
{
	lasterr_code=0;
	errbuffer[0]=0;
}

int DLL_EXPORT getVersion_MBR2(char *msg,int nb_wanted)
/* Return the release number, e.g. "2.05a"  */
{
	strncpy(msg,SYNTH_VERSION,nb_wanted);
	msg[nb_wanted-1]=0;
  
	return nb_wanted;
}
