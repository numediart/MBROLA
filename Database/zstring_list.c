/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    zstring_list.c
 * Purpose: list of zstring (can be used for renaming phoneme pairs)
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <01/09/26 16:51:08 mbrola>
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
 * 26/9/01 : the type of PhonemeCode was really too small
 */

#include <string.h>
#include "zstring_list.h"
#include "mbralloc.h"
#include "vp_error.h"

#if defined(ROMDATABASE_STORE) || defined(ROMDATABASE_INIT)
#include "rom_handling.h"
#endif


ZStringList* init_ZStringList()
/* Basic constructor, initialize to empty list */
{
	ZStringList* my_zl= (ZStringList*) MBR_malloc(sizeof(ZStringList));
	nb_elem(my_zl)=0;
	nb_available(my_zl)= PACKET_ALLOCATION; /* must be EVEN */
	zlist(my_zl)= (PhonemeName *) MBR_malloc( sizeof(PhonemeName) * nb_available(my_zl));
	return my_zl;
}

void close_ZStringList(ZStringList* zl)
/* Release the memory */
{
	int i;
  
	for(i=0; i<nb_elem(zl); i++)
		MBR_free(zlist(zl)[i]);
  
	MBR_free(zlist(zl));
	MBR_free(zl);
}

int size_ZStringList(ZStringList* zl)
/* return the size of the renaming list */
{
	return nb_elem(zl);
}

PhonemeCode find_ZStringList(ZStringList* zl, PhonemeName name)
/* 
 * find the name in the table and return its translation
 * or return PHONEME_FAIL
 */
{
	int i;
  
	for(i=0; i<nb_elem(zl); i++)
    {
		if (strcmp(name, zlist(zl)[i])==0)
			return i;
    }
	return PHONEME_FAIL;
}

PhonemeCode append_ZStringList(ZStringList* zl, PhonemeName str)
/* Add a new phoneme a returns its code */
{
	PhonemeCode code;
  
	/* enough memory */
	if (nb_elem(zl)>= nb_available(zl))
    {
		/* sanity check */
		if (nb_available(zl)+PACKET_ALLOCATION > MAX_PHONEME_NUMBER)
		{
			fatal_message(ERROR_TOOMANYPHONEMES,"Too many phonemes in the dba %i!\n",nb_elem(zl));
		}
      
		nb_available(zl)+= PACKET_ALLOCATION;
		zlist(zl)= (PhonemeName*) MBR_realloc( zlist(zl),
											   sizeof(PhonemeName) * nb_available(zl) );
    }
	code= nb_elem(zl);
	zlist(zl)[ nb_elem(zl)++ ]= MBR_strdup(str); 
  
	return code;
}

PhonemeCode encode_ZStringList(ZStringList* zl, PhonemeName str)
/* finds the translation of 'str' (if not available add a new code) */
{
	PhonemeCode code= find_ZStringList(zl,str);
	if (code == PHONEME_FAIL)
    { 
		code= append_ZStringList(zl, str); 
    }
  
	return code;
}

PhonemeName decode_ZStringList(ZStringList* zl, PhonemeCode code)
/* back to the string from the code, the compiler should expand it as a macro */
{
	return( zlist(zl)[code] );
}

/**********************************
 * DEDICATED TO PHONEME RENAMING
 **********************************/

bool append_rename_ZStringList(ZStringList* zl, PhonemeName old_name, PhonemeName new_name, bool multi)
/* Add a new renaming pair to the list 
 * If multi is True, it's a multiset
 *
 * Return False if the key was allready present (if it's not multiset)
 */
{
	PhonemeName renamed;
  
	if ( (!multi) &&
		 ((renamed=find_rename_ZStringList(zl,old_name)) != NULL))
    {
		warning_message(ERROR_RENAMING, "Can't map %s to %s. Already mapped to %s\n",
						old_name, new_name, renamed);
		return False;
    }
  
	append_ZStringList(zl, old_name); 
	append_ZStringList(zl, new_name); 
	return True;
}

bool parse_ZStringList(ZStringList* my_zl, PhonemeName rename_string, bool multi)
/* 
 * Parsing the renaming pairs from a string 
 * Returning False means wrong initializer (check LastErr)
 *
 * multi= True means multiset. If False, introducing a renaming pair with
 * the same key is an error (returns False)
 */
{
	PhonemeName name1;
	PhonemeName name2;

	name1= strtok( rename_string, " \n");
	name2= strtok( NULL, " \n");
  
	while ( (name1!= NULL) &&
			(name2!= NULL))
	{
		if (! append_rename_ZStringList(my_zl,name1,name2,multi))
			return False;  /* fatal with append */		
		
		name1= strtok(NULL, " \n");
		name2= strtok(NULL, " \n");
	}
  
	if (name1 != NULL)
	{
		warning_message(ERROR_RENAMING,"Wrong renaming list at %s\n",name1);
		return False;
	}
  
	return True;
}


PhonemeName find_rename_ZStringList(ZStringList* zl, PhonemeName name)
/* 
 * find the name in the table and return its translation
 * or return NULL
 */
{
	int i;
  
	for(i=0; i<nb_elem(zl); i+=2)
    {
		if (strcmp(name, zlist(zl)[i])==0)
			return zlist(zl)[i+1];
    }
	return NULL;
}

#ifdef ROMDATABASE_STORE
/***************************
 * Relevant to ROM Databases 
 ***************************/

void file_flush_ROM_ZStringList(ZStringList* zl, FILE* rom_file)
/* Dump the hash table in a ROM image */
{
	int i;

	file_flush_ROM_align16(rom_file);
	file_flush_ROM_int16( nb_elem(zl), rom_file);
	for(i=0; i< nb_elem(zl); i++)
		file_flush_ROM_Zstring( zlist(zl)[i],rom_file);
}
#endif

#ifdef ROMDATABASE_INIT

ZStringList* init_ROM_ZStringList(void** input_ptr)
/*
 * Initialize the hash table from a ROM image
 */
{
	int i;
	ZStringList* my_zl= (ZStringList*) MBR_malloc(sizeof(ZStringList));
  
	ptr_ROM_align16(*input_ptr);
	*input_ptr= read_ROM_int16( (int16*) &nb_elem(my_zl), *input_ptr);
	nb_available(my_zl)=nb_elem(my_zl);
  
	/* Allocate directly the right size */
	zlist(my_zl)= (PhonemeName *) MBR_malloc( sizeof(PhonemeName) * 
											  nb_available(my_zl));
	for(i=0; i< nb_elem(my_zl); i++)
    {
		*input_ptr= read_ROM_Zstring( &zlist(my_zl)[i], *input_ptr);
    }
	return my_zl;
}

void close_ROM_ZStringList(ZStringList* zl)
/*
 * Close the ROM image (fewer mallocs than in the regular one) 
 */
{
	MBR_free( zlist(zl) );
	MBR_free( zl );
}

#endif

