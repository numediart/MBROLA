/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    rom_database.h
 * Time-stamp: <00/03/29 23:57:41 pagel>
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

#ifndef _ROM_DATABASE_H
#define _ROM_DATABASE_H

#include "database.h"

#ifdef ROMDATABASE_STORE
void file_flush_ROM_Database(Database* dba, char* out_name);
/* Dump the database structure into a ROM image */

void file_flush_ROM_header(Database* dba, FILE* rom_file);
/*
 * Dump the core of a database structure into a ROM image 
 * This part is generally common, whatever encoding method
 */

void file_flush_ROM_DatabaseBasic(Database* dba, FILE* rom_file);
/* Nothing much to do with a RAW database */

#endif

#ifdef ROMDATABASE_INIT

void close_ROM_DatabaseBasic(Database* dba);
/*
 * Nothing much to desallocate or close when you're in ROM
 */

void init_ROM_header(Database* my_dba);
/* Collective function for different coding types */

Database* init_ROM_DatabaseBasic(Database* dba);
/* 
 * Basic version, simply init a pointer on wave forms = Check there's no coding 
 * Returning NULL means error
 */

Database* init_ROM_Database(void* input_ptr);
/*
 * Initialize a database from a ROM image (polymorphic result)
 */
#endif

#endif
