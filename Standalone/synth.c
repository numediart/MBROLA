/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    synth.c
 * Purpose: Main function of the MBROLA speech synthesizer.
 * Authors: Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <00/03/30 03:30:20 pagel>
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
 * 10/01/97: Created as well as library.c to offer 2 compilation modes.
 *           front end with a main() function.
 *
 * 20/10/97: flag to get the database textual information
 * 
 * 11/06/98: new object oriented scheme
 *
 * 28/08/98: phoneme renaming
 *
 * 11/09/98: reset signal handling
 *
 * 20/10/98: phoneme renaming in a file for compatibility (Festival)
 *
 * 27/03/00: Rom databases dumping + initialization from ROM image for
 *           debugging purposes
 */

#include "common.h"
#include "zstring_list.h"
#include <stdio.h>

#define void_(x) if (x) {}

#if defined(__GLIBC__)
#include <getopt.h>
#else

#if defined(TARGET_OS_DOS) || defined(TARGET_OS_SUN4) || defined (__STRICT_ANSI__)
#include <string.h>
#include "Posix/getopt.c"
#endif

#if defined(__linux__) || defined(__EMX__)
#include <getopt.h>
#endif

#endif

#include "common.h"
#include "database.h"
#include "diphone.h"
#include "mbrola.h"
#include "audio.h"
#include "input_file.h"
#include "parser_input.h"
#include "synth.h"

#if defined(ROMDATABASE_INIT) || defined(ROMDATABASE_STORE)
#include "rom_database.h"
#endif

/* 
 * In standalone mode, input and ouput through files
 */
FILE *output_file;  /* Audio output file (can be stdout)  */

/* main objects */
Database* my_dba;
Mbrola* my_brole;
Parser* my_parse;

/* Globals initialized from Initialization file, or from command line */
float freq_ratio=1.0;
float time_ratio=1.0;
float volume_ratio=1.0;
bool smoothing=True;
bool no_error=False;		  /* True if phoneme error resistant */
char* comment_symbol=NULL;   /* init from command line */
char* flush_symbol=NULL;     /* init from rename file  */
float my_pitch;              /* default pitch value */
uint16 voice_len=0;          /* Voice sampling rate */
ZStringList* rename_list;     /* phoneme renaming */
ZStringList* clone_list;      /* phoneme cloning */

#ifdef SIGNAL

/*
 * Global modified by the signal handler
 *  -> indicate to flush what's being played 
 * used as "reset play" in programs like emacs speak
 */
volatile sig_atomic_t  must_flush=False;

void reset_handler(int signum)
/* To catch the users signal (means flush the input) */
{
	warning_message(1,"Got a reset signal !\n");
  
	must_flush=True;
	signal (signum, reset_handler); /* rearm */
}
#endif

/* Different functions dedicated to global initializations */

void set_comment_symbol(char* sym)
{
	/* Avoid overlapping */
	if (comment_symbol)
		MBR_free(comment_symbol);
	comment_symbol= MBR_strdup(sym);
}

void set_flush_symbol(char* sym)
{
	/* Avoid overlapping */
	if (flush_symbol)
		MBR_free(flush_symbol);
	flush_symbol= MBR_strdup(sym);
}

void set_volume_ratio(char* val)
{
	if ((volume_ratio=(float) atof(val))<=0.0f)
    {
		printf("Error in the format of the volume ratio : %s\n",val);
		exit(1);
    }
}

void set_freq_ratio(char* val)
{
	if ((freq_ratio=(float) atof(val))<=0.0f)
    {
		printf("Error in the format of the frequency ratio : %s\n",val);
		exit(1);
    }
}

void set_time_ratio(char* val)
{
	if ((time_ratio=(float) atof(val))<=0.0f)
    {
		printf("Error in the format of the time ratio : %s\n",val);
		exit(1);
    }
}

void set_voice_len(char* val)
{
	if ((voice_len= (int16) atoi(val))<=0)
    {
		fprintf(stderr,"Error in the format of the Voice ratio : %s\n",val);
		exit(1);
    }
  
	if ( (voice_len<1000) ||
		 (voice_len>48000))
    {
		fprintf(stderr,"The Voice ratio is a sampling frequency : %s\n",val);
		exit(1);
    }
}

void parse_init_file(char* rename_file_name)
/* Parse a file mapping phonemes onto other with ;;RENAME a A ... */
{
	FILE* rename_file= fopen(rename_file_name,OPENRT);
	char buffer[255];
	char source[255];
	char dest[255];
  
	if (rename_file==NULL)
		fatal_message(ERROR_DBNOTFOUND,"Error with %s renaming file !\n",rename_file_name);
  
	/* Read a line with quite a loose syntax ... */
	while (fgets(buffer,sizeof(buffer),rename_file))
    {
		char* pos;  /* position of the command start, ignoring whatever preceedes */
		
		debug_message2("LINE: %s\n",buffer);
		
		/* skip whatever comment char in the header */
		if ( (pos= strstr(buffer,"RENAME")) != NULL)
		{
			sscanf(pos,"RENAME %s %s",source,dest);
			debug_message3("rename %s to %s\n",source,dest);
			append_rename_ZStringList(rename_list, source, dest, True);
		}
		else if ( (pos= strstr(buffer,"CLONE")) != NULL)
		{
			sscanf(pos,"CLONE %s %s",source,dest);
			debug_message3("clone %s %s\n",source,dest);
			append_rename_ZStringList(clone_list, source, dest, False);
		}
		else if ( (pos= strstr(buffer,"FLUSH")) != NULL)
		{
			sscanf(pos,"FLUSH %s",source);
			debug_message2("flush %s\n",source);
			set_flush_symbol(source);
		}
		else if ( (pos= strstr(buffer,"COMMENT")) != NULL)
		{
			sscanf(pos,"COMMENT %s",source);
			debug_message2("comment %s\n",source);
			set_comment_symbol(source);
		}
		else if ( (pos= strstr(buffer,"TIME")) != NULL)
		{
			sscanf(pos,"TIME %s",source);
			debug_message2("time %s\n",source);
			set_time_ratio(source);
		}
		else if ( (pos= strstr(buffer,"FREQ")) != NULL)
		{
			sscanf(pos,"FREQ %s",source);
			debug_message2("freq %s\n",source);
			set_freq_ratio(source);
		}
		else if ( (pos= strstr(buffer,"VOLUME")) != NULL)
		{
			sscanf(pos,"VOLUME %s",source);
			debug_message2("volume %s\n",source);
			set_volume_ratio(source);
		}
		else if ( (pos= strstr(buffer,"VOICE")) != NULL)
		{
			sscanf(pos,"VOICE %s",source);
			debug_message2("voice %s\n",source);
			set_voice_len(source);
		}
		else if ( (pos= strstr(buffer,"IGNORE")) != NULL)
		{
			debug_message1("ignore\n");
			no_error=True;
		}
    }
}

void process_one_file(Mbrola* mb, char *file_name)
/*
 * Send one file on the output
 */
{
	FILE *command_file; /* File providing the phonetic input (can be stdin) */ 
	int stream_eof;	/* To make the difference between flush and eof */
	Input* my_input; /* to build the new parser */
	Parser* my_parse;

	/* A - as input file means STDIN */
	if (!strcmp(file_name,PIPESYMB))
		command_file=stdin;
	else if ((command_file=fopen(file_name,OPENRT)) == NULL)
		fatal_message(ERROR_DBNOTFOUND,"Error with %s input file !\n",file_name);
  
	/*
	 * HERE WE GOOOOOOoooo
	 * Loop and flush until the end of pho file is reached
	 */

	my_input= init_InputFile(command_file);
	my_parse= init_ParserInput(my_input, 
							   sil_phon(my_dba), 
							   my_pitch, 
							   time_ratio, freq_ratio,
							   comment_symbol, flush_symbol);
	set_parser_Mbrola(mb,my_parse);
	do
    {
		reset_Mbrola(mb);
		stream_eof=Synthesis(mb);
		fflush(output_file);
    }
	while (stream_eof!=PHO_EOF);
  
	my_parse->close_Parser(my_parse);	/* close the parser     */
	my_input->close_Input(my_input);  /* and the input stream */
	if (command_file!=stdin) fclose(command_file);
}


/* 
 * Function name says all !
 */
int main(int argc, char **argv)
{
	WaveType file_format;	     /* File format for the audio output */
	int argpos;
	int c;
	bool info=False;           /* True if textual information requested */

#ifdef ROMDATABASE_STORE
	bool romdatabase_store= False; /* True if we build a ROM dump */ 
#endif

#ifdef ROMDATABASE_INIT
	void *romdatabase_pointer=NULL; /* Pointer to a ROM dump: used for debugging */
	bool romdatabase_init= False;   /* True if the dba is a ROM dump */
#endif

	rename_list= init_ZStringList();     /* phoneme renaming */
	clone_list=  init_ZStringList();      /* phoneme cloning */

#ifdef SIGNAL
	/*
	 * Plug the reset signal handler (but don't overide parent's Signal Ignore
	 */
	if (signal (SIGUSR1, reset_handler) == SIG_IGN)
		signal (SIGUSR1, SIG_IGN);

#endif

	/* Paranoid test ! */
	if ((sizeof(uint8)!=1)
		|| (sizeof(int16)!=2)
		|| ( sizeof(int32)!=4))
		fatal_message(ERROR_DBWRONGARCHITECTURE,"Architecture PANIC!\n");

	if (argc==1)
    {
		printf(
			"\n"
			" MBROLA " SYNTH_VERSION " - speech synthesizer\n"
			" Author : Thierry Dutoit with help of AC/DC adaptator Vincent Pagel\n"
			" Copyright (c) 1995-2018 Faculte Polytechnique de Mons (TCTS lab) - T.Dutoit\n"
			" \n"
			" This program is free software: you can redistribute it and/or modify\n"
			" it under the terms of the GNU Affero General Public License.\n"
			" See the https://www.gnu.org/licenses/agpl-3.0.html for more details.\n"
			" \n"
			" Note that MBROLA language databases are provided with different licences.\n"
			" See licencing information for them provided at " DB_ADDRESS "\n"
			" \n"
			" For more information look at " WWW_ADDRESS "\n"
			" or contact mbrola@tcts.fpms.ac.be.\n"
			" \n"
			" THIS SOFTWARE CARRIES NO WARRANTY, EXPRESSED OR IMPLIED.  THE USER\n"
			" ASSUMES ALL RISKS, KNOWN OR UNKNOWN, DIRECT OR INDIRECT, WHICH INVOLVE\n"
			" THIS SOFTWARE IN ANY WAY. IN PARTICULAR, THE AUTHOR DOES NOT TAKE ANY\n"
			" COMMITMENT IN VIEW OF ANY POSSIBLE THIRD PARTY RIGHTS.\n\n"
			" U.S. Patent Number 5,987,413\n"
			"\n"
			" For help type : mbrola -h\n"
			"\n");
		return 0;
    }

	/* Read the switches */
	while ((c=getopt(argc, argv, "+v:t:f:l:c:F:R:C:I:shiewW"))>0)
		switch(c)
		{
		case 'i':
			info=True;
			break;
		  
		case 'e':
			no_error=True;
			break;
		  
		case 'c':
			set_comment_symbol(optarg);
			break;
		  
		case 'F':
			set_flush_symbol(optarg);
			break;

		case 'v':
			set_volume_ratio(optarg);
			break;
		  
		case 'f':
			set_freq_ratio(optarg);
			break;

		case 't':
			set_time_ratio(optarg);
			break;
		  
		case 'l':
			set_voice_len(optarg);
			break;

		case 'C':
			parse_ZStringList(clone_list, optarg, True);
			break;

		case 'R':
			parse_ZStringList(rename_list, optarg, False);
			break;

		case 'I':
			parse_init_file(optarg);
			break;
		  
		case 's':
			smoothing=False;
			break;
		  
		case 'h':
			printf("\n"
				   " USAGE: %s [COMMAND LINE OPTIONS] database pho_file+ output_file\n"
				   "\n"
				   "A " PIPESYMB " instead of pho_file or output_file means stdin or stdout\n"
				   "Extension of output_file ( raw, au, wav, aiff ) tells the wanted audio format\n"
				   "\nOptions can be any of the following:\n"
				   "-i    = display the database information if any\n"
				   "-e    = IGNORE fatal errors on unknown diphone\n"
				   "-c CC = set COMMENT char (escape sequence in pho files)\n"
				   "-F FC = set FLUSH command name\n"
				   "-v VR = VOLUME ratio, float ratio applied to ouput samples\n", argv[0]);
            printf("-f FR = FREQ ratio, float ratio applied to pitch points\n"
				   "-t TR = TIME ratio, float ratio applied to phone durations\n"
				   "-l VF = VOICE freq, target freq for voice quality\n"
				   "-R RL = Phoneme RENAME list of the form ""a A b B ...""\n"
				   "-C CL = Phoneme CLONE list of the form ""a A b B ...""\n\n"
				   "-I IF = Initialization file containing one command per line\n"
				   "        CLONE, RENAME, VOICE, TIME, FREQ, VOLUME, FLUSH, COMMENT,\n"
				   "        and IGNORE are available\n"
#ifdef ROMDATABASE_STORE
				   "-W    = store the datbase in ROM format\n"
#endif
#ifdef ROMDATABASE_INIT
				   "-w    = the database in a ROM dump\n"
#endif
				   "\n");
			return 0;
#ifdef ROMDATABASE_STORE
		case 'W':
			romdatabase_store=True;
			break;
#endif
#ifdef ROMDATABASE_INIT
		case 'w':
			romdatabase_init=True;
			break;
#endif
		default:
			printf("Error, option %c not recognized, try -h for help\n",c);
			return 1;
		}
  
	argpos=optind;

	if (argpos >= argc)
		fatal_message(ERROR_COMMANDLINE,
					  "Not enough arguments, try -h for help\n");
  
#ifdef ROMDATABASE_INIT
	if (romdatabase_init)
    {
		long rom_size;
		FILE* rom_file;
		char out_name[1024];
		sprintf( out_name, "%s.rom", argv[argpos] );
      
		/* Open the ROM image for simulation */
		rom_file= fopen(out_name,"rb");
		if (rom_file==NULL)
			fatal_message(ERROR_DBNOTFOUND,"Unable to read %s!\n",out_name);

		fseek(rom_file,0,SEEK_END);
		rom_size=ftell(rom_file);
		rewind(rom_file);
      
		/* Allocate the big ROM area and read data in it */
		romdatabase_pointer= MBR_malloc(rom_size);
		void_(fread(romdatabase_pointer, 1, rom_size, rom_file ));
		fclose(rom_file);
      
		my_dba= init_ROM_Database(romdatabase_pointer);
    }
	else
		/* else normal initialization */
#endif
    {
#ifndef ROMDATABASE_PURE
		/* initialize the database with rename and clone */
		my_dba= init_rename_Database(argv[argpos], rename_list, clone_list);
#endif
    }

	if (!my_dba)
    {
		fatal_message(ERROR_COMMANDLINE,
					  "All database initializations failed\n");
    }
  
	/* not usefull anymore */
	if (rename_list)
		close_ZStringList(rename_list);
	if (clone_list)
		close_ZStringList(clone_list);
  
#ifdef ROMDATABASE_STORE
	if (romdatabase_store)
    {
		char out_name[1024];
		sprintf( out_name, "%s.rom", dbaname(my_dba) );
      
		/* Dump a copy of the database into a file to get a ROM image */
		file_flush_ROM_Database( my_dba, out_name);
		fprintf(stderr, "ROM image saved in %s\n", out_name);      
    }
#endif  /* ROMDATABASE_STORE */
  
	/* do not connect any parser yet */
	my_pitch= (float)Freq(my_dba) / (float)MBRPeriod(my_dba);
	my_brole= init_Mbrola(my_dba);
	set_volume_ratio_Mbrola(my_brole,volume_ratio);
	set_smoothing_Mbrola(my_brole,smoothing);
	set_no_error_Mbrola(my_brole,no_error);
  
	if (voice_len!=0)
		set_voicefreq_Mbrola(my_brole,voice_len);
  
	if (info)
    {
		/* Information requested */
		char *local;
		int size;
		int index=0;
      
		size=getDatabaseInfo(my_dba,NULL,0,index);
		while (size!=0)		  
		{
			local=(char *) MBR_malloc(size);
			getDatabaseInfo(my_dba,local,size,index);
			printf("\n%s\n",local);
			MBR_free(local);
			index+=1;
			size=getDatabaseInfo(my_dba,NULL,0,index);
		}
    }
  
	if (argpos+2 >= argc)
    {
		/* Not fatal so that we close everything before leaping */
		warning_message(ERROR_COMMANDLINE,
						"Not enough arguments, try -h for help\n");
    }
	else
    {
		/* A - as output file means STDOUT -> may be followed by a format switch */
		if (!strncmp(argv[argc-1],PIPESYMB,1))
			output_file=stdout;
		else if ((output_file=fopen(argv[argc-1],"wb")) == NULL)
			fatal_message(ERROR_OUTFILE,"Error with %s output file !\n",argv[argc-1]);
      
		/* 
		 * The stream header ( length is unknown at this time )
		 */
		file_format=find_file_format(argv[argc-1]);
      
		write_header(file_format,
					 audio_length(my_brole),
					 get_voicefreq_Mbrola(my_brole),
					 output_file);
      
		/* Process the files one by one */
		argpos++;  
		while (argpos<argc-1)
			process_one_file(my_brole, argv[argpos++]);
      
		/* 
		 * Close the streams and make correction on the header if possible
		 */
		if (output_file!=stdout) 
		{ /* Go to the begining of the output and write the complete header 
		   *	( we now know the audio_length)
		   */
			fseek(output_file,0,SEEK_SET); 
			write_header(file_format,
						 audio_length(my_brole),
						 get_voicefreq_Mbrola(my_brole),
						 output_file);
			fclose(output_file);
		}
    }
  
	close_Mbrola(my_brole);		       /* Close the engine */
	my_dba->close_Database(my_dba);  /* ... the database */
  
	if (flush_symbol)
		MBR_free(flush_symbol);
	if (comment_symbol)
		MBR_free(comment_symbol);
  
#ifdef ROMDATABASE_INIT
	if (romdatabase_init)
    { MBR_free(romdatabase_pointer); }
#endif  
  
	return 0;
}
