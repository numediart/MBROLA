/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    database.h
 * Time-stamp: <2000-04-07 16:48:29 pagel>
 * Purpose: diphone database management
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
 * 29/02/96 : Created. Put here everything concerning the speech database 
 *
 * 02/02/97 : Replacement diphones enabled (T. Dutoit)
 *
 * 08/09/97 : New format -> phonemes are written as Zstring (old limit in 
 *            size was 2 char/phoneme)
 *            New policy for silence definition
 *
 * 20/10/97 : Database information = Zstrings at the end of the dba
 *
 * 05/11/97 : BACON info for 2.05c
 *
 * 25/02/98 : Database formats cleaning
 *            drop old compatibility checks with 2.02 and so on
 *            Escape sequence in database_info for non printable information
 *            archiving the database construction parameters 
 *
 * 03/03/98 : Non posix platforms can't realloc on a NULL pointer
 *
 * 23/06/98 : Polymorphic databases !
 *            A cloning mechanism for multi-instanciation databases
 *
 * 28/08/98 : in my way to C/ANSI, dropped the 'const' definition
 *            (that would require extern variables :-(
 *            Thus "const FrameType VOICING_MASK=2;"
 *            becomes "#define VOICING_MASK 2"
 *
 * 20/03/00 : keep tracks of the longest diphone in the database to
 *            remove static limits from mbrola engine
 *
 *            Detects when opening directories .... it's a FAQ because error 
 *            message was not clear, adding a Database extension like ".dba"
 *            should do
 *
 *            Rom databases included (database can be FILE* or int16* depending on
 *            the database mode)
 *            
 *            25% extra space in the hashtable enhances search
 */

#ifndef _DATABASE_H
#define _DATABASE_H

#include "audio.h"
#include "diphone.h"
#include "hash_tab.h"

#define DIPHONE_RAW 1	  /* The diphone wave database is raw */
#define ROM_MASK 128      /* The Coding tag of the database indicate if it's in ROM */

#define INFO_ESCAPE 0xFF     /* Escape code in database informations (prevents from displaying) */

/*
 * Frame types in the MBR analysed database
 */
typedef uint8 FrameType;

#define VOICING_MASK 2 /* Voiced/Unvoiced mask  */
#define TRANSIT_MASK 1 /* Stationary/Transitory mask */
#define NV_REG 0          /* unvoiced stable state */
#define NV_TRA TRANSIT_MASK    /* unvoiced transient    */
#define V_REG VOICING_MASK    /* voiced stable state   */
#define V_TRA (VOICING_MASK | TRANSIT_MASK)  /* voiced transient      */

/*
 * Main type
 */
typedef struct Database Database;

typedef bool (*getdiphone_DatabaseFunction)(Database* dba, DiphoneSynthesis *diph);
typedef void (*close_DatabaseFunction)(Database* dba);
typedef Database* (*init_DatabaseFunction)(Database* dba);

struct Database
{
	/* Polymorphic structure, depends on Coding */
	void* self;	

	/* Virtual function for diphone wave loading	*/
	getdiphone_DatabaseFunction getdiphone_Database;

	/* Virtual function to release the memory */
	close_DatabaseFunction close_Database;

	uint8 Coding;	  /* Type of coding DIPHONE_RAW, or BACON */
	int16 Freq;		  /* Sampling frequency of the database */
	uint8 MBRPeriod;	  /* Period of the MBR analysis */
	int16 nb_diphone;       /* Number of  diphones in the database */

	int32 SizeMrk;	  /* Size of the pitchmark part */
	FrameType *pmrk;           /* The whole pitch marks database   */

	int32 SizeRaw;	  /* Size of the wave part      */
	int32 RawOffset;         /* Offset for raw samples in database  */

	uint8 max_frame;        /* Maximum number of frames encountered for a diphone in the dba */
	int16 max_samples;      /* Size of the diphone buffer= 0 means let me manage it myself */

	int32 Magic[2];	  /* Magic header of the database */
	char Version[6];	  /* Version of the database    */

	PhonemeName sil_phon;	  /* Silence symbol in the database */

	HashTab *diphone_table;    /* Diphone index table */
  
	ZStringList* info;    /* information strings */

	char *dbaname;          /* name of the diphone file */
	void *database;         /* diphone wave file or base pointer to wave data, depending on dba type */
};

/* Convenient macros */
#define dbaname(PDatabase)  PDatabase->dbaname

/* Those 2 macros access the same field */
#define rom_wave_ptr(PDatabase) (PDatabase->database)
#define database(PDatabase)  ((FILE*)PDatabase->database)

#define nb_diphone(PDatabase) PDatabase->nb_diphone
#define RawOffset(PDatabase) PDatabase->RawOffset
#define Coding(PDatabase) PDatabase->Coding
#define Freq(PDatabase) PDatabase->Freq
#define MBRPeriod(PDatabase) PDatabase->MBRPeriod
#define SizeMrk(PDatabase) PDatabase->SizeMrk
#define SizeRaw(PDatabase) PDatabase->SizeRaw
#define max_frame(PDatabase) PDatabase->max_frame
#define max_samples(PDatabase) PDatabase->max_samples
#define Magic(PDatabase) PDatabase->Magic
#define Version(PDatabase) PDatabase->Version
#define sil_phon(PDatabase) PDatabase->sil_phon
#define info(PDatabase) PDatabase->info
#define pmrk(PDatabase) PDatabase->pmrk
#define diphone_table(PDatabase) PDatabase->diphone_table


#ifndef ROMDATABASE_PURE
/*
 * Functions relying on FILE based databases
 */


/*
 * Three parts of the Database header
 */

	bool ReadDatabaseHeader(Database* dba);
/*  Reads the diphone database header , and initialize variables  */

bool ReadDatabaseIndex(Database* dba);
/* 
 * Read the index table of diphones, and put them in the hash table
 */

bool ReadDatabasePitchMark(Database* dba);
/* Load pitch markers (Voiced/Unvoiced, Transitory/Stationnary) */

bool ReadDatabaseInfo(Database* dba);
/*
 * Extract textual information from the database if any
 */

/*
 * Initialisation and loading of Diphones -> depend on database Coding
 * Returning NULL means fail (check LastError)
 */
Database* init_DatabaseBasic(Database* dba);
/* 
 * Basic version, read raw waves = Check there's no coding 
 * Returning NULL means error
 */

void close_DatabaseBasic(Database* dba);
/* Release the memory allocated for the in-house BACON decoder */

Database* init_Database(char* dbaname);
/* Generic initialization, calls the appropriate constructor 
 * Returning NULL means fail (check LastError)
 */

Database* init_rename_Database(char* dbaname, ZStringList* rename, ZStringList* clone);
/* 
 * A variant of init_Database allowing phoneme renaming on the fly 
 * Returning NULL means fail (check LastError)
 *
 * rename and clone can be NULL to indicate there's nothing to change
 *
 * Renaming is a ONCE consuming operation (the database is changed
 * at loading) -> it involves a complete reconstruction of the hash table
 * but nothing else at run-time
 */

#endif /* ROMDATABASE_PURE */

#ifdef MULTICHANNEL_MODE

Database* copyconstructor_Database(Database* dba);
/* Creates a copy of a diphone database so that many synthesis engine 
 * can use the same database at the same time (duplicate the file handler)
 *
 * Returning NULL means fail (check LastError)
 *
 * Highly recommended with multichannel mbrola, unless you can guaranty
 * mutually exclusive access to the getdiphone function
 */
#endif

/*
 * Available to everybody
 */

int getDatabaseInfo(Database* dba, char* msg, int size, int index);
/*
 * Retrieve the ith info message, NULL means get the size
 */ 

bool init_common_Database(Database* dba, DiphoneSynthesis *diph);
/*
 * Common initialization shared among all database types
 */

bool getdiphone_DatabaseBasic(Database* dba, DiphoneSynthesis *ds);
/* 
 * Basic loading of the diphone specified by diph. Stores the samples 
 * Return False in case of error
 */

#endif
