/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    rom_handling.c
 * Time-stamp: <00/03/29 22:56:46 pagel>
 * Purpose: basic initialization from ROM
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
 * 23/03/00 : Created.
 */

#include "rom_handling.h"

#ifdef ROMDATABASE_INIT

void* read_ROM_uint8(uint8* val, void* input_ptr)
/* initialize val from input_ptr, return next position */
{
	*val= * ((uint8 *) input_ptr);
	return (void*) ((size_t)input_ptr + sizeof(uint8));
}

void* read_ROM_int16(int16* val, void* input_ptr)
/* initialize val from input_ptr, return next position */
{
	*val= * ((int16 *) input_ptr);
	return (void*) ((size_t) input_ptr + sizeof(int16));
}

void* read_ROM_int32( int32* val, void* input_ptr )
/* initialize val from input_ptr, return next position */
{ 
	*val= *((int32 *) input_ptr);
	return (void*) ((size_t)input_ptr + sizeof(int32));
}

void* read_ROM_array(void** any, int size_item, int nb_item, void* input_ptr )
/* 
 * initialize an array containing elements of size 'size_elements' 
 * from a ROM image starting at input_ptr 
 */
{
	*any= input_ptr;
	return (void*) ((size_t)input_ptr + size_item*nb_item);
}

void* read_ROM_Zstring(char** val, void* input_ptr)
/* initialize val from input_ptr, return next position */
{
	char* zstring= (char*) input_ptr;
	*val= zstring;

	while ( *zstring != (char) 0 )
		zstring++;

	/* next one */
	zstring++;
  
	return (void*) zstring;
}

#endif /* ROMDATABASE_INIT */


/***********************************************************
 * Serie of functions used to flush a memory image to a file
 * for later ROM usage
 ***********************************************************/

#ifdef ROMDATABASE_STORE

void file_flush_ROM_uint8(uint8 val, FILE* output)
/* initialize val from input_ptr, return next position */
{
	fwrite( &val, sizeof(uint8), 1, output);
}

void file_flush_ROM_int16(int16 val, FILE* output)
/* initialize val from input_ptr, return next position */
{
	fwrite( &val, sizeof(int16), 1, output);
}

void file_flush_ROM_int32(int32 val,  FILE* output)
/* initialize val from input_ptr, return next position */
{
	fwrite( &val, sizeof(int32), 1, output);
}

void file_flush_ROM_array(void* any, int size_item, int nb_item, FILE* output)
/* initialize an array containing elements of size 'size_elements' 
 * from a ROM image starting at input_ptr 
 */
{
	fwrite(any, size_item, nb_item, output);
}

void file_flush_ROM_Zstring(char* val, FILE* output)
/* initialize val from input_ptr, return next position */
{
	fprintf(output,"%s",val);
	fputc(0,output);
  
}

/*************************************************
 * Those functions align ROM file dumps on multiple of 2 (int16) or of 4 (int32)
 * -> compulsory for to avoid BUS_SIG some processors 
 * -> access more efficient for other CPU
 */

void file_flush_ROM_align16(FILE*output)
{
	long file_pos= ftell(output);

	/* Odd position */
	if ( file_pos & 0x1 )
		fputc( 0x00, output);
}

void file_flush_ROM_align32(FILE*output)
{
	long file_pos= ftell(output);
	int i=  (4 - (file_pos & 0x3))%4;

	/* Not a multiple of 4 -> dummy fillers */
	while (i--)
		fputc( 0x00, output);
}

#endif /* ROMDATABASE_STORE */
