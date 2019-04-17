/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    little_big.c
 * Purpose: IO little_endian aware
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
 * 09/04/98 : Created from older audio.c
 * Put here input output function depending on the little endian or
 * big endian format 
 */

#include "little_big.h"

#define void_(x) if (x) {}

/* 
 * Read and write operations with/without byte swapping
 */
void write_int16(int16 value, FILE *output_file)
{
	fwrite(&value,sizeof(int16), 1, output_file);
}

void write_int32(int32 value, FILE *output_file)
{
	fwrite(&value,sizeof(int32), 1, output_file);  
}

void write_int16_swapped(int16 value, FILE *output_file)
{
	value= (int16) ( ((value & 0xFF00) >>8) | ((value & 0xFF) <<8) );
  
	fwrite(&value,sizeof(int16), 1, output_file);
}

void write_int32_swapped(int32 value, FILE *output_file)
{
	value=(((value&0xFF)<<24) | \
		   ((value&0xFF00)<<8) | \
		   ((value&0xFF0000)>>8) | \
		   ((value>>24)&0xFF));
	fwrite(&value,sizeof(int32), 1, output_file);  
}

void read_int16(int16 *value, FILE *output_file)
{
	void_(fread(value,sizeof(int16), 1, output_file));
}

void read_uint16(uint16 *value, FILE *output_file)
{
	void_(fread(value,sizeof(uint16), 1, output_file));
}

size_t read_int16buffer(int16 *ptr, size_t nitems, FILE *stream)
{
	return fread(ptr,sizeof(int16),nitems,stream);
}

void read_int32(int32 *value, FILE *output_file)
{
	void_(fread(value,sizeof(int32), 1, output_file));  
}

void read_int16_swapped(int16 *value, FILE *output_file)
{
	void_(fread(value,sizeof(int16), 1, output_file));
  
	*value= (int16) ( ((*value & 0xFF00) >> 8) | ((*value & 0xFF) << 8) );
}

size_t read_int16buffer_swapped(int16 *ptr, size_t nitems, FILE *stream)
{
	size_t nb_read=fread(ptr,sizeof(int16),nitems,stream);
	swab( (char*) ptr, (char*) ptr, sizeof(int16)*nb_read);
	return(nb_read);
}

void read_uint16_swapped(uint16 *value, FILE *output_file)
{
	void_(fread(value,sizeof(uint16), 1, output_file));
  
	*value= (int16) (((*value & 0xFF00) >> 8) | ((*value & 0xFF) << 8));
}

void read_int32_swapped(int32 *value, FILE *output_file)
{
	void_(fread(value,sizeof(int32), 1, output_file));  
	*value=(((*value&0xFF)<<24) | \
			((*value&0xFF00)<<8) | \
			((*value&0xFF0000)>>8) | \
			((*value>>24)&0xFF));
}
