/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    rom_database.c
 * Time-stamp: <01/09/26 16:32:17 mbrola>
 * Purpose: diphone database management (from ROM dumps)
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
 * 24/03/00 : Created.
 *
 * 29/03/00 : Memory alignment issue ... On a SUN reading int32
 *   requires multiple of 4, int16 requires a multiple of 2... 
 *   Otherwise BUS error. Anyway alignment is good for everybody and
 *   adds very little dummy chars.
 */

#include "rom_handling.h"
#include "rom_database.h"
#include "little_big.h"

#ifdef BACON
#include "database_bacon.h"
#endif

#ifdef CEBAB
#include "database_cebab.h"
#endif


#ifdef ROMDATABASE_INIT
/***********************************************
 * ROM constructor depends on the database type
 ***********************************************/

/* Table of database constructors */
init_DatabaseFunction init_ROM_tab[ ]= { init_ROM_DatabaseBasic, /* type 0 , was DatabaseOld -> exactly the same as RAW dba */
										 init_ROM_DatabaseBasic, /* type 1 */
#ifdef BACON
										 init_ROM_DatabaseBacon, /* type 2 */
#else
										 init_ROM_DatabaseBasic,
#endif
#ifdef CEBAB
										 init_ROM_DatabaseCebab, /* type 3 */
#else
										 init_ROM_DatabaseBasic,
#endif
};

#define NB_ROM_DATABASE_TYPE (sizeof(init_ROM_tab)/sizeof(init_DatabaseFunction))

#endif /* ROMDATABASE_INIT */

#ifdef ROMDATABASE_STORE
/*************************************************************
 * Storing method of a ROM image depends on the database type
 *************************************************************/

/* Table of database constructors */
typedef void (*file_flush_ROM_DatabaseFunction)(Database* dba, FILE* rom_file);

file_flush_ROM_DatabaseFunction file_flush_ROM_tab[ ]= { file_flush_ROM_DatabaseBasic, /* type 0 , was DatabaseOld */
														 file_flush_ROM_DatabaseBasic, /* type 1 */
#ifdef BACON
														 file_flush_ROM_DatabaseBacon, /* type 2 */
#else
														 file_flush_ROM_DatabaseBasic,
#endif
#ifdef CEBAB
														 file_flush_ROM_DatabaseCebab, /* type 3 */
#else
														 file_flush_ROM_DatabaseBasic,
#endif
};

void file_flush_ROM_header(Database* dba, FILE* rom_file)
/*
 * Dump the core of a database structure into a ROM image 
 * This part is generally common, whatever encoding method
 */
{
	int32 round_size;

	/* Rest of the header */
	file_flush_ROM_uint8( MBRPeriod(dba), rom_file);
  
	/* Then magically, we're aligned on 32 ! */
	file_flush_ROM_align32(rom_file); /* ... but sanity anyway */ 
	file_flush_ROM_int16( Freq(dba), rom_file);
	file_flush_ROM_int16( nb_diphone(dba), rom_file);
  
	file_flush_ROM_int32( SizeMrk(dba), rom_file);
	file_flush_ROM_int32( SizeRaw(dba), rom_file);
	file_flush_ROM_int32( RawOffset(dba), rom_file);
  
	/* The diphone hash table (auto-align) */
	file_flush_ROM_HashTab( diphone_table(dba), rom_file);

	/* Database information */
	file_flush_ROM_ZStringList( info(dba), rom_file);
  
	/* 
	 * The rest is not aligned but we dont't care 
	 */
  
	/* pitch marks */
	round_size= (SizeMrk(dba)+3)/4;  /* round to upper value */
	file_flush_ROM_array( pmrk(dba), sizeof(FrameType), round_size, rom_file);
  
	file_flush_ROM_uint8( max_frame(dba), rom_file);
  
	/* Silence phoneme */
	file_flush_ROM_Zstring( sil_phon(dba), rom_file);
}

void file_flush_ROM_DatabaseBasic(Database* dba, FILE* rom_file)
/* Nothing much to do with a RAW database */
{
	long pos_read;
  
	/* The common part */
	file_flush_ROM_header(dba, rom_file);

	/* Align with the forthcoming samples */
	file_flush_ROM_align16(rom_file);
  
	/* Read and write the whole WAVE chunk */
	fseek(database(dba), RawOffset(dba), SEEK_SET);
	pos_read= RawOffset(dba);
  
	while ( (!feof(database(dba))) &&
			(pos_read < RawOffset(dba)+SizeRaw(dba)))
    {
		int16 buffer[255];

		/* Respect endianness when reading codebook indexes */
		int nb_read= readl_int16buffer( buffer, 255, database(dba));
		fwrite(buffer, sizeof(int16), nb_read, rom_file);
		pos_read+=nb_read;
    }
}

void file_flush_ROM_Database(Database* dba, char* out_name)
/* Dump the database structure into a ROM image */
{
	FILE* rom_file= fopen(out_name, "wb");
	if (rom_file==NULL)
    {
		fatal_message(ERROR_DBNOTFOUND,"FATAL ERROR : cannot save to file %s !\n",out_name);
    }
  
	/* The name of the database... useless, only kept for information purpose */
	file_flush_ROM_Zstring( dbaname(dba), rom_file);
  
	/* Write "MBROLA" "2.069" */
	file_flush_ROM_align32(rom_file);
	file_flush_ROM_array( Magic(dba), sizeof(int32), 2, rom_file);
	file_flush_ROM_array( Version(dba), sizeof(char), 6, rom_file);
  
	/* The database code name include a ROM tag */
	file_flush_ROM_uint8( Coding(dba) | ROM_MASK , rom_file); 
 
	file_flush_ROM_tab[ Coding(dba) ](dba, rom_file);
	fclose(rom_file);
}
#endif /* ROMDATABASE_STORE */

#ifdef ROMDATABASE_INIT

void close_ROM_DatabaseBasic(Database* dba)
/*
 * Nothing much to desallocate or close when you're in ROM
 */
{
	/* fewer desalloc in Database Information */
	if ( info(dba) )
		close_ROM_ZStringList( info(dba) );
  
	/* close upper structures of the hash table */
	if (diphone_table(dba))
		close_ROM_HashTab( diphone_table(dba) );
  
	/* No pitch marks, no file, no silence phoneme */
  
	/* The structure itself */
	MBR_free(dba);
}

void init_ROM_header(Database* my_dba)
/* Collective function for different coding types */
{
	int32 round_size;
	void* input_ptr= rom_wave_ptr(my_dba);

	input_ptr= read_ROM_uint8( &MBRPeriod(my_dba), input_ptr);

	/* Then magically, we're aligned on 32 ! */

	ptr_ROM_align32(input_ptr); /* sanity */
	input_ptr= read_ROM_int16( &Freq(my_dba), input_ptr);
	input_ptr= read_ROM_int16( &nb_diphone(my_dba), input_ptr);
  
	input_ptr= read_ROM_int32( &SizeMrk(my_dba), input_ptr);
	input_ptr= read_ROM_int32( &SizeRaw(my_dba), input_ptr);
	input_ptr= read_ROM_int32( &RawOffset(my_dba), input_ptr);  

	/* The diphone hash table (autoalign) */
	diphone_table(my_dba)= init_ROM_HashTab( &input_ptr);

	/* Database information */
	info(my_dba)= init_ROM_ZStringList( &input_ptr );

	/* 
	 * The rest is not aligned but we dont't care 
	 */

	/* Compressed pitch marks (2bits/mark) */
	round_size= (SizeMrk(my_dba)+3)/4;  /* round to upper value */
	input_ptr= read_ROM_array( (void*) &pmrk(my_dba), 
							   sizeof(FrameType),
							   round_size,
							   input_ptr);
  
	input_ptr= read_ROM_uint8( &max_frame(my_dba), input_ptr);

	/* Silence phoneme */
	input_ptr= read_ROM_Zstring(& sil_phon(my_dba), input_ptr);
  
	/* Pointer to the start of the waves in ROM */
	rom_wave_ptr(my_dba)= input_ptr;

	/* Size of the buffer that will be allocated in Diphonesynthesis */
	max_samples(my_dba)= MBRPeriod(my_dba) * max_frame(my_dba);
}

Database* init_ROM_DatabaseBasic(Database* dba)
/* 
 * Basic version, simply init a pointer on wave forms = Check there's no coding 
 * Returning NULL means error
 */
{
	debug_message1("init_ROM_DatabaseBasic\n");
  
	/* init virtuals */
	dba->getdiphone_Database= getdiphone_DatabaseBasic;
	dba->close_Database= close_ROM_DatabaseBasic;
  
	/* A basic dba contains RAW waveforms */
	if ( (Coding(dba) & (ROM_MASK-1)) != DIPHONE_RAW )
    {
		dba->close_Database(dba);
		fatal_message( ERROR_BINNUMBERFORMAT,
					   "PANIC: This program can't decode your database %i\n",Coding(dba));
		return NULL;
    }
  
	init_ROM_header(dba);

	/*
	 * Size of the buffer that will be allocated in Diphonesynthesis 
	 * is null as ROM_DatabaseBasic returns pointer on buffers in ROM
	 */
	max_samples(dba)= 0; 
  
	/* Align for INT16 audio samples */
	ptr_ROM_align16( rom_wave_ptr(dba) );
  
	debug_message1("done init_ROM_DatabaseBasic\n");
	return(dba);
}

Database* init_ROM_Database(void* input_ptr)
/*
 * Initialize a database from a ROM image
 * Returning NULL means fatal error (check LastErr)
 */
{
	int i;
	Database* my_dba;
  
	debug_message1("init_ROM_Database\n");
  
	my_dba= (Database*) MBR_malloc(sizeof(Database));
	my_dba->database= NULL;
	sil_phon(my_dba)= NULL;
	info(my_dba)= NULL;
	max_frame(my_dba)= 0;
	pmrk(my_dba)= NULL;
	diphone_table(my_dba)= NULL;

	/* will be changed later on, depending on the dba type, it's here for premature exists */
	my_dba->close_Database= close_ROM_DatabaseBasic; 
  
	/* The name of the database... useless, only kept for information purpose */
	input_ptr= read_ROM_Zstring( &dbaname(my_dba), input_ptr);
  
	/* Read "MBROLA" */
	ptr_ROM_align32(input_ptr);
	input_ptr= read_ROM_int32( &Magic(my_dba)[0], input_ptr);
	input_ptr= read_ROM_int32( &Magic(my_dba)[1], input_ptr);
  
	if ( (strncmp("MBROLA",(char *)Magic(my_dba),6))
		 || (Magic(my_dba)[0] != MAGIC_HEADER))
    {
		fatal_message(ERROR_DBWRONGVERSION,
					  "PANIC: Binary number format error\n"
					  "You are probably using a version of %s incompatible\n"
					  "with your machine architecture.\n"
					  ,dbaname(my_dba));
		return NULL;
    }
  
	/* Read version number "2.01" */
	for(i=0;i<6;i++)
		input_ptr= read_ROM_uint8( (uint8*) &(Version(my_dba)[i]), input_ptr);
  
	if (strcmp(Version(my_dba),SYNTH_VERSION)>0)
    {
		fatal_message(ERROR_DBWRONGARCHITECTURE,
					  "PANIC: Can't cope with databases coming from the future\n");
		return NULL;
    }
  
	input_ptr= read_ROM_uint8( &Coding(my_dba), input_ptr);
  
	/* Remove ROM bit and check we have the decoder */
	if ( (Coding(my_dba)& (ROM_MASK-1)) > NB_ROM_DATABASE_TYPE )
    {
		my_dba->close_Database(my_dba);
		fatal_message( ERROR_BINNUMBERFORMAT,
					   "PANIC: This program can't decode your database code %i\n", Coding(my_dba));
		return NULL;
    }
  
	debug_message1("virtual init_ROM_Database\n");
  
	/* Yes it is that simple !! Watch your step */
	rom_wave_ptr(my_dba)=input_ptr;
	return init_ROM_tab[ Coding(my_dba)& (ROM_MASK-1) ](my_dba);
}

#endif
