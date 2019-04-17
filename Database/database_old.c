/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    database_old.c
 * Time-stamp: <00/03/30 01:37:55 pagel>
 *
 * Purpose: Decode raw formats before 2.05 release, here for compatibility purpose
 *          Use pretty much RAW functions
 * 
 * Author: Vincent Pagel
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
 * History:
 *
 *  25/06/98 : Created from 2.06 code
 *  20/03/00 : Add support for max_frame parsing + new hash table scheme
 */

#include "database.h"

#define void_(x) if (x) {}

#ifndef ROMDATABASE_PURE

/*********************************************************
 * OLD TYPES:   Better lock it up and throw away the key *
 *********************************************************/

/* 
 * Structure of the diphone database (as stored in file)
 * Used before 2.02a
 */
typedef struct
{
	char left[2],right[2];	/* Name of the diphone     */
	int32 pos_wave;		      /* position in SPEECH_FILE */
	int16 halfseg;		      /* position of center of diphone */
	uint16 pos_pm;		      /* index in PITCHMARK_FILE */
	uint8  nb_frame;    		/* Number of pitch markers */
	char dummy[3];				/* Alignment on multiple of 4 */
} DiphoneEvenOlderFile;

/* 
 * Structure of the diphone database (as stored in file)
 * Used from release 2.02a to release 2.05
 *
 * Since 2.05 there is no more struct related to the 
 * index datas, we just read the info sequentially
 */
typedef struct
{
	char left[2],right[2];	/* Name of the diphone     */
	int16 halfseg;		/* position of center of diphone */
	uint8  nb_frame;		/* Number of pitch markers */
	uint8  nb_wframe;    /* index in PITCHMARK_FILE */
} DiphoneOldFile;

/* Specifies a diphone that is the copy of another one */
typedef struct
{
	char left[2],right[2];	/* Name of the diphone     */
	char leftr[2],rightr[2];	/* Name of the replacement diphone     */
} DiphoneReplace;


/*******************************************************
 * OLD FUNCTIONS: don't even ruin your eyesight on it  *
 *******************************************************/

Database* init_DatabaseOld(Database* dba)
/*
 * Initializes the old ones!
 */
{
	int i;
  
	int32 indice_pm=0;
	int32 indice_wav=0;
	bool even_older;
  
	/* Allocate one more byte for the trailing 0 */
	PhonemeName left_cell= (char *) MBR_malloc(3);
	PhonemeName right_cell= (char *) MBR_malloc(3);
  
	int16 halfseg_cell;
	int32 pos_wave_cell;
	int32 pos_pm_cell;
	uint8 nb_frame_cell;
  
	debug_message2("Old compatibility mode : %i\n",Coding(dba));
	warning_message(WARNING_UPGRADE,"Think of upgrading your database!\n");
  
	/* Except some oddities for ReadDatabaseIndex and PitchMark, once it's 
	 * loaded it works like RAW databases
	 */
	dba->getdiphone_Database= getdiphone_DatabaseBasic;
	dba->close_Database= close_DatabaseBasic;
  
	/* Check version < 2.02 or  2.02 <= version < 2.05 */
	if (strcmp("2.02",Version(dba))>0)
    {
		nb_diphone(dba)= (int16) (nb_diphone(dba) / sizeof(DiphoneEvenOlderFile));
		even_older=True;
    }
	else /* From 2.02 to 2.05 */
    {	/* New format allow more diphone in a database than previously */
		even_older=False;
    }
  
	/* 
	 * Read the index table of diphones, and put them in the hash table
	 */
  
	diphone_table(dba)=init_HashTab(nb_diphone(dba));		  /* initialize hash table */
  
	/* Insert diphones one by one in the hash table */
	for(i=0; (indice_pm!=SizeMrk(dba)) && (i<nb_diphone(dba)); i++)
    {
		if (even_older)
			/* Neolithic 16 bytes structures */
		{ 
			DiphoneEvenOlderFile one_oldfcell;  /* Cell on file */
	  
			void_(fread(&one_oldfcell, sizeof(one_oldfcell),1, database(dba)));
	  
			/* PC format ! */
#ifdef BIG_ENDIAN  
			one_oldfcell.pos_wave= (((one_oldfcell.pos_wave&0xFF)<<24) |
									((one_oldfcell.pos_wave&0xFF00)<<8) |
									((one_oldfcell.pos_wave&0xFF0000)>>8) |
									((one_oldfcell.pos_wave>>24)&0xFF));
	  
			one_oldfcell.pos_pm = ( ((one_oldfcell.pos_pm&0xFF00) >>8)  |
									((one_oldfcell.pos_pm&0xFF) <<8));
#endif
			strncpy( left_cell , one_oldfcell.left,2);
			strncpy( right_cell , one_oldfcell.right,2);
	  
			nb_frame_cell= one_oldfcell.nb_frame;
			halfseg_cell=  one_oldfcell.halfseg;
			pos_wave_cell= one_oldfcell.pos_wave;
			pos_pm_cell=   one_oldfcell.pos_pm;
		}		
		else
		{	/* Renaissance 8 bytes structures */
			DiphoneOldFile one_fcell;		  /* Cell on file */
	  
			void_(fread(&one_fcell,sizeof(one_fcell),1,database(dba)));
	  
			strncpy( left_cell, one_fcell.left,2);
			strncpy( right_cell, one_fcell.right,2);
	  
			nb_frame_cell= one_fcell.nb_frame;
			halfseg_cell= one_fcell.halfseg;
	  
			pos_pm_cell= indice_pm;
			indice_pm+=one_fcell.nb_frame;
	  
			pos_wave_cell= indice_wav;
			indice_wav+= (long) one_fcell.nb_wframe* (long) MBRPeriod(dba);
		}
      
		/* add a trailing 0 to the name */
		left_cell[2]=  0;
		right_cell[2]= 0;

		/* One world, one vision, one diphone database (the PC one) -> swap */
#ifdef BIG_ENDIAN  
		halfseg_cell= ( (( halfseg_cell & 0xFF00) >>8)  |
						(( halfseg_cell & 0xFF) <<8));
#endif
      
		debug_message8("%i Diph %s-%s nbframe=%i poswav=%i pospm=%i halfseg=%i\n",
					   i,
					   left_cell,
					   right_cell,
					   nb_frame_cell,
					   pos_wave_cell,
					   pos_pm_cell,
					   halfseg_cell);
      
		add_HashTab(diphone_table(dba), 
					left_cell,
					right_cell,
					pos_wave_cell, 
					halfseg_cell,
					pos_pm_cell,
					nb_frame_cell);

		/* Keep a record of the longest diphone (to allocate oversized temporary buffer) */
		if (nb_frame_cell*1.5 > max_frame(dba))
		{ max_frame(dba)=nb_frame_cell*1.5; }
    }
      
	/* The last diphone of the database is _-_ */
	sil_phon(dba)= MBR_strdup(left_cell);
	debug_message2("Init silence with %s\n", left_cell);
  
	/* dutoit 02/02/97 
	 * Check if we've reached the end of the diphone table
	 * The rest are replacement diphones, they don't correspond
	 * to new samples, but duplicate existing diphones
	 */
	for( ; i<nb_diphone(dba); i++)
    {
		DiphoneInfo cell;
		DiphoneReplace replace;		  /* Replacement on file */
		int position;
      
		void_(fread(&replace, sizeof(replace), 1, database(dba)));
		strncpy(left_cell, replace.leftr, 2);
		strncpy(right_cell, replace.rightr, 2);
		left_cell[2]=0;
		right_cell[2]=0;
      
		position= search_HashTab(diphone_table(dba), left_cell, right_cell);
      
		/* Sanity check: the target exist */
		if (position==NONE)
		{
			fatal_message(ERROR_CANTDUPLICATESEGMENT,
						  "Fatal error: Can't duplicate %s-%s segment\n",
						  left_cell,right_cell);
			return NULL;
		}
      
		debug_message4("Copy %s-%s position %i ",
					   left_cell,
					   right_cell,
					   position);
      
		strncpy(left_cell, replace.left, 2);
		left_cell[2]=0;
      
		strncpy(right_cell, replace.right, 2);
		right_cell[2]=0;
      
		debug_message3("into %s-%s \n", left_cell, right_cell);
      
		/* Sanity check: the target allready exist */
		position= search_HashTab(diphone_table(dba), 
								 left_cell, right_cell);
		if (position!=NONE)
		{
			fatal_message(ERROR_CANTDUPLICATESEGMENT,
						  "Fatal error: duplicate %s-%s segment allready exist\n",
						  left_cell, 
						  right_cell);
			return NULL;
		}
      
		cell= *content(diphone_table(dba), position);
		add_HashTab(diphone_table(dba), left_cell, right_cell,
					pos_wave(cell),
					halfseg(cell),
					pos_pm(cell),
					nb_frame(cell) );		
    }
  
	MBR_free(left_cell);
	MBR_free(right_cell);
  
#ifdef DEBUG_HASH
	tuning_HashTab(diphone_table(dba));
#endif
  
	/*
	 * Load pitch markers (Voiced/Unvoiced, Transitory/Stationnary)
	 */
	if (even_older)
    {
		pmrk(dba)= (unsigned char *) MBR_malloc(SizeMrk(dba));
		void_(fread(pmrk(dba), sizeof(char),SizeMrk(dba), database(dba)));
		RawOffset(dba)=ftell(database(dba));
    }
	else
    {
		if (!ReadDatabasePitchMark(dba))
		{
			return NULL;
		}
    }
  
	if (!ReadDatabaseInfo(dba))
    {
		return NULL;
    }

	/* Size of the buffer that will be allocated in Diphonesynthesis */
	max_samples(dba)= MBRPeriod(dba) * max_frame(dba);
  
	debug_message1("done Compatibility init\n");
	return(dba);
}

#endif /* ROMDATABASE_PURE */
