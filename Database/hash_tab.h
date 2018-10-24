/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    hash_table.h
 * Time-stamp: <00/03/29 23:48:28 pagel>
 * Purpose: coalescent hashing table
 * Authors:  Vincent Pagel and Alain Ruelle
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
 * 29/02/96 : Created. Implement a coalescent hashing of the diphone database
 *
 * 29/01/97 : Generalization to phoneme names longer than 2 chars
 *
 * 01/09/97 : Alain Ruelle adds close_hash to release the memory
 *
 * 17/03/00 : VP adds an auxiliary phoneme encoding table (for ROM databases)
 * the policy is that maintaining Phoneme names with char* for a ROM
 * frozen hash structure is foolish, while maintaining an index in a
 * phoneme table is sensible (ROM image of the hashtable can be frozen
 * since it doesn't contain any "pointers" - without mentionning that
 * pointers on phoneme names were desesperately redundants). The cost
 * of that change is: when initializing a database it takes a sequential
 * or dichotomic search in the auxiliary table to translate phoneme
 * names (sequential at the moment as number of phonemes are generally
 * small).
 *
 * Ultimately may be that some of you wish to even completely remove
 * char* phonemes and use uint8 instead and spare the "malloc" "free"
 * dance on phoneme names during Parser input. Yes phoneme names is a
 * convention we can do without .... except for human readability and
 * debugging. Another advantage of PhonemeCode everywhere would be
 * that the hash_table would be reduced to computing x*N+y access in a 
 * matrix!!  
 */

#ifndef _HASH_TAB_H
#define _HASH_TAB_H

#include "diphone_info.h"
#include "zstring_list.h"

/* Return when the index is not found in the hash table */
#define NONE -1

/* Used to mark a hash cell as empty */
#define EMPTY 255

/* Wrapper structure */
typedef struct
{
  DiphoneInfo content;        /* Hashing information */
  uint8 hit;			   /* survey value for number of collisions */
  int16 next_one;	   /* link to the next database cell */
} HashInfo;

/* The whole diphone database          */
typedef struct
{
  ZStringList * auxiliary_tab; /* Table of phoneme names */      

  HashInfo *hash_tab;     /* Hashing information */
  int16 nb_item;	  /* Number of elements in hash_tab */
  int16 first_free;	  /* First position free from the end of the table */
#ifdef DEBUG_HASH
  int16 tot_nb_coup;
  int16 tot_coup;
#endif
} HashTab;

/* Convenient macros */
#define nb_item(Tab) Tab->nb_item
#define first_free(Tab) Tab->first_free
#define next_one(Tab,Index) Tab->hash_tab[Index].next_one
#define hit(Tab,Index) Tab->hash_tab[Index].hit
#define content(Tab,Index) (&(Tab->hash_tab[Index].content))

#define auxiliary_tab(Tab) (Tab->auxiliary_tab)
#define auxiliary_tab_val(Tab,Index) ( decode_ZStringList(auxiliary_tab(Tab), Index) )

HashTab *init_HashTab(int16 nb_item);
/* Initialize a void hash_table */

void close_HashTab(HashTab *hash_tab);
/* Empty and release the hash_table */

int16 searchdiph_HashTab(const HashTab *hash_tab, DiphoneInfo* one_cell);
/* 
 * Return the reference number of a diphone in the diphone database
 * Hash table search -> return NONE=-1 if the value is not present
 */

int16 search_HashTab(const HashTab *hash_tab,const PhonemeName left, const PhonemeName right);
/* 
 * Return the reference number of a diphone in the diphone database
 * Hash table search -> return NONE=-1 if the value is not present
 */

void add_HashTab(HashTab *hash_tab, PhonemeName new_left, PhonemeName new_right,
		 int32 new_pos_wave, int16 new_halfseg, int32 new_pos_pm, uint8 new_nb_frame );
/* Add a new reference in the diphone table  */

HashTab* diphone_rename_HashTab(HashTab* hash_tab, ZStringList* rename);
/*
 * Rename all occurences of diphones containing the phoneme X to phone Y
 * in the hash table according to rename which contains X-Y pairs
 * 
 * WARNING 1:  it costs some CPU and memory moves ....
 *
 * WARNING 2: This operation fundamentaly change the rank of elements
 * in the hash table, so
 *
 *      FORGET ALL YOUR POINTER TO ELEMENTS OF THE TABLE
 */

HashTab* diphone_clone_HashTab(HashTab* hash_tab, ZStringList* clone);
/* Make a copy of all occurences of diphones containing the phoneme 
 * X to phoneme Y according to the clone list which contains X-Y pairs
 *
 * The clone list contains only ONE occurence of each phoneme
 */

#ifdef DEBUG_HASH
void tuning_HashTab(HashTab *hash_tab);
/* Function for debug and tuning purpose  */
#endif

#ifdef ROMDATABASE_STORE
void file_flush_ROM_HashTab( HashTab* tab,  FILE* rom_file);
/* Dump the hash table in a ROM image */
#endif

#ifdef ROMDATABASE_INIT
HashTab* init_ROM_HashTab(void** input_ptr);
/*
 * Initialize the hash table from a ROM image
 */

void close_ROM_HashTab(HashTab *hash_tab);
/*
 * Close the ROM image (fewer malloc that in regular one)
 */
#endif

#endif
