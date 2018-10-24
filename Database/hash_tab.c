/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    hash_table.c
 * Time-stamp: <00/03/30 00:17:20 pagel>
 * Purpose: coalescent hashing table
 * Author:  Vincent Pagel and Alain Ruelle
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
 *
 * 29/03/00: VP adds memory alignment for ROM databases
 */

#include <math.h>
#include "common.h"
#include "database.h"
#include "mbrola.h"
#include "diphone_info.h"
#include "hash_tab.h"

#if defined(ROMDATABASE_STORE) || defined(ROMDATABASE_INIT)
#include "rom_handling.h"
#endif

HashTab* init_HashTab(int16 nb_item)
/* Initialize an empty hash_table */
{
	int i;
	HashTab *result;
  
	result= (HashTab *) MBR_malloc(sizeof(HashTab));
	nb_item(result)= nb_item;
	result->hash_tab= (HashInfo *) MBR_malloc(nb_item*sizeof(HashInfo));
  
	/* Mark all the cells as unoccupied */
	for(i=0; i<nb_item; i++)
		hit(result,i)=EMPTY;
  
	/* first free place for coalescent add */
	first_free(result)= (int16) (nb_item- 1);
  
	auxiliary_tab(result)= init_ZStringList();
  
#ifdef DEBUG_HASH
	result->tot_nb_coup=0;
	result->tot_coup=0;
#endif
	return(result);
}

void close_HashTab(HashTab *hash_tab)
/* Empty and release the hash_table  ~ruelle */
{
	if (hash_tab)
    {
		if (auxiliary_tab(hash_tab))
			close_ZStringList(auxiliary_tab(hash_tab));
      
		if (hash_tab->hash_tab)
			MBR_free(hash_tab->hash_tab);
      
		MBR_free(hash_tab);
    }
}

int16 mix(int32 index, int16 nb_diphone)
/*
 * Homogeneization to avoid performance drops depending on nb_diphone
 * if we had computed mult % nb_diphone
 */
{
	double temp;
  
	temp= index * 0.6180339887; /* (sqrt(5)-1)/2 */
	temp-= floor(temp);
	temp*= nb_diphone;
	return( (int16) temp); 
}

void add_HashTab(HashTab *hash_tab, PhonemeName new_left, PhonemeName new_right,
				 int32 new_pos_wave, int16 new_halfseg, int32 new_pos_pm, uint8 new_nb_frame )
/* Add a new reference in the diphone table  */
{
	DiphoneInfo* one_cell;
	int16 first_free;
	int16 hash_value= mix( hash_DiphoneInfo(new_left, new_right), nb_item(hash_tab));
	int16 chosen;
  
	/* The primary choice is free? */
	if (hit(hash_tab,hash_value)==EMPTY)
    {
		chosen= hash_value;
		next_one(hash_tab, chosen)=NONE;
      
#ifdef DEBUG_HASH
		debug_message2("primary choice FREE %i\n",hash_value);
#endif
    }
	else
    {
#ifdef DEBUG_HASH
		debug_message3("-> %i hits at %i\n",hit(hash_tab,hash_value),hash_value);
#endif
		first_free=first_free(hash_tab);
      
		/* First free cell in the hash table */
		for( ; hit(hash_tab,first_free)!=EMPTY ; first_free--);
      
		/* put the cell in the linked list */
		chosen=first_free;
		next_one(hash_tab,chosen)= next_one(hash_tab,hash_value);
		next_one(hash_tab,hash_value)=chosen;

		/* Cumulate hits */
		hit(hash_tab,hash_value)++;
      
		first_free--;			  /* It's now occupied */
		first_free(hash_tab)=first_free;
    }

	/* Put effective data in the chosen cell */
	hit(hash_tab,chosen)=1;
	one_cell= content(hash_tab, chosen);
	pos_wave(*one_cell)=new_pos_wave;
	pos_pm(*one_cell)=new_pos_pm;
	halfseg(*one_cell)=new_halfseg;
	nb_frame(*one_cell)=new_nb_frame;
	left(*one_cell)= encode_ZStringList(auxiliary_tab(hash_tab), new_left);
	right(*one_cell)= encode_ZStringList(auxiliary_tab(hash_tab), new_right);
}

bool equalkey_HashTab(const HashTab* ht, int16 hash_value, const PhonemeName left, const PhonemeName right)
/*
 * True if the keys for hashing are equal
 */
{
	return ( (strcmp( auxiliary_tab_val(ht, left(*content(ht,hash_value))) , left) == 0) &&
			 (strcmp( auxiliary_tab_val(ht, right(*content(ht,hash_value))),  right) == 0) );
}

int16 search_HashTab(const HashTab *hash_tab,const PhonemeName left,const PhonemeName right)
/* 
 * Return the reference number of a diphone in the diphone database
 * Hash table search -> return NONE=-1 if the value is not present
 */
{
#ifdef DEBUG_HASH
	int nb_coup=1;
#endif
	int16 hash_value=mix( hash_DiphoneInfo(left,right),
						  nb_item(hash_tab));
  
	/* The primary choice doesn't exist */
	if (hit(hash_tab,hash_value)==EMPTY)
		hash_value=NONE;
  
	while ((hash_value!=NONE) &&
		   (!equalkey_HashTab( hash_tab, hash_value, left, right)))
    {
#ifdef DEBUG_HASH 
		nb_coup++;
#endif
		hash_value= next_one(hash_tab,hash_value);
    }
#ifdef DEBUG_HASH
	hash_tab->tot_nb_coup+= nb_coup;
	hash_tab->tot_coup++;
	debug_message3(" NBTRIES=%i AVERAGE=%f\n",
				   nb_coup,
				   (float) hash_tab->tot_nb_coup/(float)hash_tab->tot_coup);
#endif
	return(hash_value);  
}

int16 searchdiph_HashTab(const HashTab *hash_tab, DiphoneInfo* di)
/* 
 * Return the reference number of a diphone in the diphone database
 * Hash table search -> return NONE=-1 if the value is not present
 */
{
	return search_HashTab(hash_tab, 
						  auxiliary_tab_val(hash_tab, left(*di)), 
						  auxiliary_tab_val(hash_tab, right(*di)));
}

HashTab *diphone_rename_HashTab(HashTab *hash_tab, ZStringList* rename)
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
{
	int i;
	PhonemeName new_name;
	HashTab *hash_temp; /* a new hash table to reorder the renamed elements */
  
	/* no renaming at all, avoid a painfull life to the hash table */
	if ( (!rename) || 
		 (size_ZStringList(rename) == 0))
		return hash_tab;
  
	/* old and new tables have the same size */
	hash_temp=init_HashTab( nb_item(hash_tab) );
  
	/* Take one by one, rename and add to the new table */
	for(i=0; i<nb_item(hash_tab); i++)
    {
		if (hit(hash_tab,i)!=EMPTY)
		{
			DiphoneInfo di= *content(hash_tab,i);
			 
			/* Try renaming on left phoneme of a diphone */
			new_name= find_rename_ZStringList(rename, 
											  auxiliary_tab_val(hash_tab, left(di)) );
			if (new_name)
			{
				/* Drop the old phoneme name */
				left(di)= encode_ZStringList(auxiliary_tab(hash_tab), new_name);
			}
			 
			/* Try renaming on right phoneme */
			new_name= find_rename_ZStringList(rename, 
											  auxiliary_tab_val(hash_tab,right(di)));
			if (new_name)
			{
				/* Drop the old name */
				right(di)=  encode_ZStringList(auxiliary_tab(hash_tab), new_name);
			}
	  
			/* Add the DiphoneInfo in the new table */
			add_HashTab(hash_temp,
						auxiliary_tab_val(hash_tab, left(di)),
						auxiliary_tab_val(hash_tab, right(di)),
						pos_wave(di),halfseg(di),pos_pm(di),nb_frame(di));
		}
    }
  
	/* 
	 * clone the new in the old one 
	 */
	close_HashTab(hash_tab);
	return hash_temp;
}

HashTab* one_diphone_clone_HashTab(HashTab* hash_tab, PhonemeName XPhone, PhonemeName YPhone)
/* Make a copy of all occurences of diphones containing the phoneme 
 * X to phoneme Y according to the clone list which contains X-Y pairs
 */
{
	int i;
	int16 nb_new_diphone=0; /* count the number of extra diphones added */
	HashTab *hash_temp; /* a new hash table to reorder the renamed elements */
  
	/* 1st pass: take one by one and count how many get copied */
	for(i=0; i<nb_item(hash_tab); i++)
    {
		if (hit(hash_tab,i)!=EMPTY)
		{
			DiphoneInfo di= *content(hash_tab,i);
			int cloneLeftRight=0;
	  
			if (strcmp(auxiliary_tab_val(hash_tab,left(di)), 
					   XPhone) == 0 )
			{ 
				nb_new_diphone++;
				debug_message5("Clone %s into %s for (%s)-%s\n",
							   XPhone,
							   YPhone,
							   auxiliary_tab_val(hash_tab, left(di)) ,
							   auxiliary_tab_val(hash_tab, right(di)));
				cloneLeftRight++;
			}
			if (strcmp(auxiliary_tab_val(hash_tab,right(di)),
					   XPhone) == 0 )
			{
				nb_new_diphone++;
				debug_message5("Clone %s into %s for %s-(%s)\n",
							   XPhone,
							   YPhone,
							   auxiliary_tab_val(hash_tab, left(di)) ,
							   auxiliary_tab_val(hash_tab, right(di)));
				cloneLeftRight++;
			}
	  
			/* There's an extra diphone if we clone both sides -> e.g. if 
			 * t is cloned to t_h, then for t-t diphone we must generate
			 * t_h-t  t-t_h but also t_h-t_h
			 */
			if (cloneLeftRight==2)
			{
				nb_new_diphone++;
	      
				debug_message5("Clone %s into %s for (%s)-(%s)\n",
							   XPhone,
							   YPhone,
							   auxiliary_tab_val(hash_tab, left(di)) ,
							   auxiliary_tab_val(hash_tab, right(di)));
			}
		}
    }
  
	debug_message2("Clone %i new targets\n",
				   nb_new_diphone );
  
	/* The new one is bigger by nb_new_diphone */
	hash_temp=init_HashTab( (int16) (nb_item(hash_tab) + nb_new_diphone) );
  
	/* 2nd pass: take one by one and introduce in the new table */
	for(i=0; i<nb_item(hash_tab); i++)
    {
		if (hit(hash_tab,i)!=EMPTY)
		{
			DiphoneInfo di= *content(hash_tab,i); /* original diphone */
			bool cloneLeftRight=0;               /* 2 if both side cloned */
	  
			/* at least, copy as is */
			add_HashTab(hash_temp,
						auxiliary_tab_val(hash_tab, left(di)),
						auxiliary_tab_val(hash_tab, right(di)),
						pos_wave(di),halfseg(di),pos_pm(di),nb_frame(di));
			 
			/* check if we build the clone on the left phoneme */
			if (strcmp(auxiliary_tab_val(hash_tab,left(di)), 
					   XPhone) == 0)
			{
				add_HashTab(hash_temp,
							YPhone,
							auxiliary_tab_val(hash_tab, right(di)),
							pos_wave(di), halfseg(di), pos_pm(di), nb_frame(di));
				cloneLeftRight++;
			}
			 
			/* check if we build the clone on the right phoneme */
			if (strcmp(auxiliary_tab_val(hash_tab,right(di)),
					   XPhone) == 0 )
			{
				add_HashTab(hash_temp, 
							auxiliary_tab_val(hash_tab, left(di)),
							YPhone,
							pos_wave(di), halfseg(di), pos_pm(di), nb_frame(di));
				cloneLeftRight++;
			}
			 
			/* check if we build the clone on both left and right */
			if (cloneLeftRight==2)
			{
				add_HashTab(hash_temp, 
							YPhone, 
							YPhone,
							pos_wave(di), halfseg(di), pos_pm(di), nb_frame(di));
			}
		}
    }
  
	/* Yo, the old table is now completely out of order 
	 * leave it, and replace with the new one
	 */
	close_HashTab(hash_tab);
	return hash_temp;
}

HashTab* diphone_clone_HashTab(HashTab *hash_tab, ZStringList* clone)
/* Make a copy of all occurences of diphones containing the phoneme 
 * X to phoneme Y according to the clone list which contains X-Y pairs
 *
 * The clone list contains only ONE occurence of each phoneme
 */
{
	int i;
  
	if (!clone)
		return hash_tab;
  
	for(i=0; i<nb_elem(clone); i+=2)
    {
		hash_tab=one_diphone_clone_HashTab(hash_tab,
										   decode_ZStringList(clone,i), 
										   decode_ZStringList(clone,i+1));
    }
  
	return hash_tab;
}

#ifdef DEBUG_HASH
void tuning_HashTab(HashTab *hash_tab)
/* Function for debug and tuning purpose  */
{
	int i;
	int incident=0;
  
	for(i=0; i< nb_item(hash_tab); i++)
		if (hit(hash_tab,i)!=EMPTY)
		{
			debug_message3("Search for %s-%s\n",
						   auxiliary_tab_val(hash_tab, left(* content(hash_tab,i) )) ,
						   auxiliary_tab_val(hash_tab, right(* content(hash_tab,i))) );
	
			if (i!=searchdiph_HashTab(hash_tab, content(hash_tab,i)))
				incident++;
		}
	debug_message3("TUNING OVER ON %i DIPHONE, %i INCIDENT\n",
				   nb_item(hash_tab),
				   incident);
  
	/* reset counters */
	hash_tab->tot_nb_coup=0;
	hash_tab->tot_coup=0;
}
#endif

/***************************
 * DEALS WITH ROM DATABASES
 ***************************/

#ifdef ROMDATABASE_STORE
void file_flush_ROM_HashTab( HashTab* tab, FILE* rom_file)
/* Dump the hash table in a ROM image */
{
	file_flush_ROM_align16( rom_file );
	file_flush_ROM_int16( nb_item(tab), rom_file);
	file_flush_ROM_int16( first_free(tab), rom_file);
	file_flush_ROM_array( tab->hash_tab, sizeof(HashInfo), nb_item(tab), rom_file);
  
	file_flush_ROM_ZStringList( auxiliary_tab(tab), rom_file);
}
#endif

#ifdef ROMDATABASE_INIT
HashTab* init_ROM_HashTab(void** input_ptr)
/*
 * Initialize the hash table from a ROM image
 */
{
	HashTab *my_ht;
  
	my_ht= (HashTab *) MBR_malloc(sizeof(HashTab));

	ptr_ROM_align16(*input_ptr);
	*input_ptr= read_ROM_int16( &nb_item(my_ht), *input_ptr);
	*input_ptr= read_ROM_int16( &first_free(my_ht), *input_ptr);
	*input_ptr= read_ROM_array( (void*) &my_ht->hash_tab, 
								sizeof(HashInfo), 
								nb_item(my_ht),
								*input_ptr);
  
	auxiliary_tab(my_ht)= init_ROM_ZStringList(input_ptr);

	return my_ht;
}

void close_ROM_HashTab(HashTab *hash_tab)
/*
 * Close the ROM image (fewer malloc than in regular one)
 */
{
	close_ROM_ZStringList( auxiliary_tab(hash_tab) );
	MBR_free(hash_tab);
}

#endif
