/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    zstring_list.h
 * Purpose: list of strings (can be used for phoneme renaming pairs)
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <01/09/26 16:40:06 mbrola>
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
 * 29/08/98 : Created as rename_list
 *  9/09/98 : leak in close_RenameList
 * 20/03/00 : Can be used for list of phonemes as well (phoneme encoding)
 *            Adds support to ROM dump and initialization
 */

#ifndef _ZSTRING_LIST_H
#define _ZSTRING_LIST_H

#include "common.h"

typedef struct
{
	uint16 nb_elem;
	uint16 nb_available;
	PhonemeName* zlist;
} ZStringList;

#define nb_elem(zl) (zl->nb_elem)
#define nb_available(zl) (zl->nb_available)
#define zlist(zl) (zl->zlist)

/* To avoid too much realloc when growing the list, allocate by packets. This number should be even for renaming pairs */
#define PACKET_ALLOCATION 6 

/* Return code when research fails */
#define PHONEME_FAIL ((PhonemeCode) -1 )

ZStringList* init_ZStringList();
/* Basic constructor, initialize to empty list */

void close_ZStringList(ZStringList* zl);
/* Release the memory */


PhonemeCode append_ZStringList(ZStringList* zl, PhonemeName str);
/* Add a new string a returns its code */

PhonemeCode encode_ZStringList(ZStringList* zl, PhonemeName str);
/* finds the translation of 'str'  (if not available add a new code) */

PhonemeName decode_ZStringList(ZStringList* zl, PhonemeCode code);
/* back to the string from the code, the compiler should expand it as a macro */

int size_ZStringList(ZStringList* zl);
/* return the size of the renaming list */

/**********************************
 * DEDICATED TO PHONEME RENAMING
 **********************************/

bool append_rename_ZStringList(ZStringList* zl, PhonemeName old_name, PhonemeName new_name, bool multi);
/* Add a new renaming pair to the list 
 * If multi is True, it's a multiset
 *
 * Return False if the key was allready present (if it's not multiset)
 */

bool parse_ZStringList(ZStringList* my_zl, PhonemeName rename_string, bool multi);
/* 
 * Parsing the renaming pairs from a string 
 * Returning False means wrong initializer (check LastErr)
 *
 * multi= True means multiset. If False, introducing a renaming pair with
 * the same key is an error (returns False)
 */

PhonemeName find_rename_ZStringList(ZStringList* zl, PhonemeName str);
/* finds the translation of 'str'. returns PHONEME_FAIL if not found */

#ifdef ROMDATABASE_STORE
void file_flush_ROM_ZStringList(ZStringList* zl, FILE* rom_file);
/* Dump the hash table in a ROM image */
#endif

#ifdef ROMDATABASE_INIT
ZStringList* init_ROM_ZStringList(void** input_ptr);
/*
 * Initialize the hash table from a ROM image
 */

void close_ROM_ZStringList(ZStringList* zl);
/*
 * Close the ROM image (fewer mallocs than in the regular one) 
 */
#endif

#endif
