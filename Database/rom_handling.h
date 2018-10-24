/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    rom_handling.h
 * Time-stamp: <2000-03-29 22:08:40 pagel>
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

#ifndef ROM_HANDLING_H
#define ROM_HANDLING_H

#include "common.h"

/***********************************************************
 * Serie of functions used to retrieve values from a ROM image
 * for ROM database initialization
 ***********************************************************/

#ifdef ROMDATABASE_INIT

void* read_ROM_uint8(uint8* val, void* input_ptr);
/* initialize val from input_ptr, return next position */

void* read_ROM_int16(int16* val, void* input_ptr);
/* initialize val from input_ptr, return next position */

void* read_ROM_int32(int32* val, void* input_ptr);
/* initialize val from input_ptr, return next position */

void* read_ROM_array(void** any, int size_item, int nb_item, void* input_ptr);
/* initialize an array containing elements of size 'size_elements' 
 * from a ROM image starting at input_ptr 
 */

void* read_ROM_Zstring(char** val, void* input_ptr);
/* initialize val from input_ptr, return next position */

/*********************************
 * Those macros align pointers on multiple of 2 (int16) or of 4 (int32)
 * -> compulsory for to avoid BUS_SIG some processors 
 * -> access more efficient for other CPU
 */
#define ptr_ROM_align(X,ROUND) {X= (void*) ((((size_t) X)+ROUND-1) & ~(ROUND-1));}
#define ptr_ROM_align16(X) ptr_ROM_align(X,2)
#define ptr_ROM_align32(X) ptr_ROM_align(X,4)

#endif /* ROMDATABASE_INIT */

/***********************************************************
 * Serie of functions used to flush a memory image to a file
 * for later ROM usage
 ***********************************************************/

/* Don't use macro to allow type checking :-> */

#ifdef ROMDATABASE_STORE

void file_flush_ROM_uint8(uint8 val, FILE* output);
/* initialize val from input_ptr, return next position */

void file_flush_ROM_int16(int16 val, FILE* output);
/* initialize val from input_ptr, return next position */

void file_flush_ROM_int32(int32 val, FILE* output);
/* initialize val from input_ptr, return next position */

void file_flush_ROM_array(void* any, int size_item, int nb_item, FILE* output);
/* initialize an array containing elements of size 'size_elements' 
 * from a ROM image starting at input_ptr 
 */

void file_flush_ROM_Zstring(char* val, FILE* output);
/* initialize val from input_ptr, return next position */

/*************************************************
 * Those functions align ROM file dumps on multiple of 2 (int16) or of 4 (int32)
 * -> compulsory for to avoid BUS_SIG some processors 
 * -> access more efficient for other CPU
 */
void file_flush_ROM_align16(FILE*output);
void file_flush_ROM_align32(FILE*output);

#endif /* ROMDATABASE_STORE */

#endif /* ROM_HANLDING_H */
