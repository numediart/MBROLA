/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:  onechannel.c
 * Purpose: A one channel Mbrola Synthesizer
 * Authors: Pagel Vincent
 * Email : mbrola@tcts.fpms.ac.be
 * Time-stamp: <2000-03-28 18:01:26 pagel>
 *
 * Copyright (c) 95 1995-2018 Faculte Polytechnique de Mons (TCTS lab)
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
 * 10/01/97: Created -> library compilation mode 
 *      No main function but a Read/Write scheme
 *
 * 10/06/97: close_MBR to release the memory on PCs !
 *  Change the behavior of write_MBR so that it doesn't attempt to write 
 *  partial strings
 *
 * 26/06/97: Changes by A. Ruelle -> 
 *      LastErrorStr2_MBR added for using the windows dll with other 
 *      languages than C (ex: Visual Basic).
 *      Removing __declspec(dllexport), and replacing it by the macro
 *      DLL_EXPORT which is set to WINAPI. With this calling convention
 *      other languages than C can call function of the dll.
 * 
 * 18/09/97: LastErrorStr2 wins against LastErrorStr... (A. Ruelle)
 * 
 * 15/10/97: 2.05a -> getFreq_MBR, getVersion_MBR for the Win DLL
 *
 * 20/10/97: 2.05b -> getDbaInfo
 *
 * 30/01/98: 2.06c -> support Mlaw coding and other Alaw or LIN8 samples.
 *
 * 17/06/98: 3.01a -> support extra low pitch ( zero padding )
 *          support the object-like ANSI/C scheme
 *          keep compatibility with the previous DLL
 *
 * 23/08/98: 3.01c -> support a bunch of get/set functions
 *                    initialization with copy and clone
 *
 * 09/09/98: 3.01d -> reset_MBR now returns a success value
 *                    first_call goes back to where it belongs (Mbrola)
 *                    and correct a bug with init/reset_MBR
 * 
 * 20/10/98: 3.01f -> flush_MBR corrected for renaming.
 */

#include "common.h"
#include "diphone.h"
#include "mbrola.h"
#include "database.h"
#include "parser_input.h"
#include "zstring_list.h"
#include "onechannel.h"

#ifdef _MSC_VER
#include "fifo.h"
#include "input_fifo.h"
#include "phonbuff.h"

void close_InputFifo(Input* in)
{
	MBR_free(in);
}
#endif

/* 
 * Globals:  main objects of the channel one 
 */
Fifo* my_fifo;     /* the char fifo for the input stream */
Input* my_input;   /* the fifo input stream for the parser */
Parser* my_parse;  /* the parser   */

Database* my_dba;  /* the database */
Mbrola* my_brole;  /* the engine   */


int DLL_EXPORT init_rename_MBR(char *dbaname,char* rename_string,char* clone_string)
/* 
 * Reads the diphone database
 * Rename and clone the list parsed from the parameter strings
 *
 * 0 if ok, error code otherwise
 */
{
	float my_pitch;
	float freq_ratio=1.0;
	float time_ratio=1.0;
	char* comment_symbol=";";
	ZStringList* rename_list=NULL;     /* phoneme renaming */
	ZStringList* clone_list=NULL;      /* phoneme cloning */

#ifdef DEBUG
	/* Log file for the DLL */
	freopen("mbrola.log","wt",stderr);
#endif

	if (rename_string)
    {
		rename_list= init_ZStringList();
		parse_ZStringList(rename_list, rename_string, False);
		if (!rename_list)
			return lastError_MBR();
    }
  
	if (clone_string)
    {
		clone_list= init_ZStringList();
		parse_ZStringList(clone_list, clone_string, True);
		if (!clone_list)
			return lastError_MBR();
    }
  
	my_dba= init_rename_Database(dbaname,rename_list,clone_list);
  
	if (my_dba==NULL)
		return lastError_MBR();

	/* not usefull after initialization */
	if (rename_list)
		close_ZStringList(rename_list);
	if (clone_list)
		close_ZStringList(clone_list);

	my_pitch= (float)Freq(my_dba) / (float)MBRPeriod(my_dba);
  
	my_fifo= init_Fifo(FIFO_SIZE);
	my_input= init_InputFifo(my_fifo);
	my_parse= init_ParserInput(my_input,
							   sil_phon(my_dba), 
							   my_pitch, 
							   time_ratio, freq_ratio,
							   comment_symbol, NULL );
    
	my_brole= init_Mbrola(my_dba);
	set_parser_Mbrola(my_brole,my_parse);
	return 0;
}

int DLL_EXPORT init_MBR(char *dbaname)
/* 
 * Reads the diphone database
 * 0 if ok, error code otherwise
 */
{
	return init_rename_MBR(dbaname,NULL,NULL);
}

void DLL_EXPORT close_MBR(void)
/*	Free all the allocated memory */
{
	close_Mbrola(my_brole);
	my_parse->close_Parser(my_parse);   /* ... the parser   */
	close_InputFifo(my_input);
	close_Fifo(my_fifo);
	my_dba->close_Database(my_dba); /* ... the database */
}

int DLL_EXPORT reset_MBR() 
/* 
 * Reset the pho buffer with residual commands -> may be used as a kind of
 * "panic" flush when a sentence is interrupted
 *
 * Return 0 if fail
 */
{  
	if (!reset_Mbrola(my_brole))
		return False;
  
	my_parse->reset_Parser(my_parse);
	return True;
}

int DLL_EXPORT readtype_MBR(void *buffer_out, int nb_wanted, AudioType sample_type)
/*
 * Reads nb_wanted samples in an audio buffer
 * Returns the effective number of samples read
 * or negative error code
 */
{ return readtype_Mbrola(my_brole,buffer_out,nb_wanted,sample_type); }


int DLL_EXPORT read_MBR(void *buffer_out, int nb_wanted)
/*
 * Reads nb_wanted samples in an audio buffer
 * Returns the effective number of samples read or 
 * negative error code
 */
{ return readtype_Mbrola(my_brole,buffer_out,nb_wanted,LIN16); }


int DLL_EXPORT write_MBR(char *buffer_in)
/* Write in the handmade fifo ! */
{ return write_Fifo(my_fifo,buffer_in) ; }


int DLL_EXPORT flush_MBR()
/*
 * Write a flush command in the stream (0 means fail). Used by client 
 * applications in case the flush symbol has been renamed
 */
{
	/* Got to break encapsulation here :-/ */
	PhoneBuff* pb= (PhoneBuff*) my_parse->self; /* my_parse comes from init_ParserInput */
	char* flush_symbol= flush_symbol(pb);
  
	if (flush_symbol)
    {
		/* Sorry for the quick hack :-( */
		int length= strlen(flush_symbol);
		char *local= MBR_strdup(flush_symbol);
		int code;
		
		/* because of sprintf(flush_symbol,"%s%%n",flush); */
		local[length-2]=0;
		strcat(local,"\n");
		code=write_MBR(local);
		MBR_free(local);
		return(code);
    }
	return(0);
}

int DLL_EXPORT getDatabaseInfo_MBR(char *msg,int nb_wanted,int index)
/* Retrieve the ith info message, NULL means get the size  */ 
{ return getDatabaseInfo(diph_dba(my_brole), msg, nb_wanted, index); }

void DLL_EXPORT setFreq_MBR(int freq)
/* Set the freq and voice ratio (change the voice tone) */
{ set_voicefreq_Mbrola(my_brole,freq); }

int  DLL_EXPORT getFreq_MBR()
/* Return the output frequency (to update soundcard) */
{ return VoiceFreq(my_brole); }

void DLL_EXPORT setNoError_MBR(int no_error)
/* Tolerance to missing diphones */
{ set_no_error_Mbrola(my_brole, no_error); }

int DLL_EXPORT getNoError_MBR()
/* Spectral smoothing or not */
{ return get_no_error_Mbrola(my_brole); }

void DLL_EXPORT setVolumeRatio_MBR(float volume_ratio)
/* Overall volume */
{ set_volume_ratio_Mbrola(my_brole, volume_ratio); }

float DLL_EXPORT getVolumeRatio_MBR()
/* Overall volume */
{ return get_volume_ratio_Mbrola(my_brole); }

void DLL_EXPORT setParser_MBR(Parser* parser)
/* drop the current parser for a new one */
{ set_parser_Mbrola(my_brole, parser); }

int DLL_EXPORT lastError_MBR()
/* Return the last error code */
{  return lasterr_code; }

int DLL_EXPORT lastErrorStr_MBR(char *buffer_err,int nb_wanted)
/* Return the last error message available */
{
	int length=strlen(errbuffer)+1;

	if (length<nb_wanted) 
		nb_wanted=length;
  
	memcpy(buffer_err,errbuffer,nb_wanted-1);
	buffer_err[nb_wanted-1]=0;

	return nb_wanted;
}

void DLL_EXPORT resetError_MBR()
/* Clear the Mbrola error buffer */
{
	lasterr_code=0;
	errbuffer[0]=0;
}

int DLL_EXPORT getVersion_MBR(char *msg,int nb_wanted)
/* Return the release number, e.g. "2.05a"  */
{
	strncpy(msg,SYNTH_VERSION,nb_wanted);
	msg[nb_wanted-1]=0;

	return nb_wanted;
}
