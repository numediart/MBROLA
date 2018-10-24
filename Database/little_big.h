/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    little_big.h
 * Time-stamp: <2000-03-28 17:55:41 pagel>
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
 * 09/04/98: Created from older audio.h
 */

#ifndef _LITTLE_BIG_H
#define _LITTLE_BIG_H

#include "common.h"

/* 
 * Check that architecture is defined and define reading and writing operations
 * depending on it ( deals with byte swapping)
 */
#ifdef LITTLE_ENDIAN
#define MAGIC_HEADER 0x4f52424d 
#define readl_int32(X,Y) read_int32(X,Y)
#define readl_int16(X,Y) read_int16(X,Y)
#define readl_int16buffer(X,Y,Z) read_int16buffer(X,Y,Z)
#define readl_uint16(X,Y) read_uint16(X,Y)
#define readb_int32(X,Y) read_int32_swapped(X,Y)
#define readb_int16(X,Y) read_int16_swapped(X,Y)
#define readb_uint16(X,Y) read_uint16_swapped(X,Y)
#define writel_int32(X,Y) write_int32(X,Y)
#define writel_int16(X,Y) write_int16(X,Y)
#define writeb_int32(X,Y) write_int32_swapped(X,Y)
#define writeb_int16(X,Y) write_int16_swapped(X,Y)
#else
#ifdef BIG_ENDIAN
#define MAGIC_HEADER 0x4d42524f
#define readl_int32(X,Y) read_int32_swapped(X,Y)
#define readl_int16(X,Y) read_int16_swapped(X,Y)
#define readl_int16buffer(X,Y,Z) read_int16buffer_swapped(X,Y,Z)
#define readl_uint16(X,Y) read_uint16_swapped(X,Y)
#define readb_int32(X,Y) read_int32(X,Y)
#define readb_int16(X,Y) read_int16(X,Y)
#define readb_uint16(X,Y) read_uint16(X,Y)
#define writel_int32(X,Y) write_int32_swapped(X,Y)
#define writel_int16(X,Y) write_int16_swapped(X,Y)
#define writeb_int32(X,Y) write_int32(X,Y)
#define writeb_int16(X,Y) write_int16(X,Y)
#else
#error "You should define BIG_ENDIAN (sun,hp,next..) or LITTLE_ENDIAN (pc,vax)"
#endif
#endif

/* 
 * Read and write operations with/without byte swapping
 */
void write_int16(int16 value, FILE *output_file);
void write_int32(int32 value, FILE *output_file);
void write_int16_swapped(int16 value, FILE *output_file);
void write_int32_swapped(int32 value, FILE *output_file);

void read_int16(int16 *value, FILE *output_file);
size_t read_int16buffer(int16 *ptr, size_t nitems, FILE *stream);
void read_uint16(uint16 *value, FILE *output_file);
void read_int32(int32 *value, FILE *output_file);
void read_int16_swapped(int16 *value, FILE *output_file);
size_t read_int16buffer_swapped(int16 *ptr, size_t nitems, FILE *stream);
void read_uint16_swapped(uint16 *value, FILE *output_file);
void read_int32_swapped(int32 *value, FILE *output_file);

#endif
