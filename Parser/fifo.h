/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    fifo.h
 * Purpose: a char fifo
 *
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
 * 18/06/98 : Created
 */

#ifndef FIFO_H
#define FIFO_H

/* Size of the standard phonetic input buffer */
#define FIFO_SIZE 8192

#define LINE_FEED 0x0a

typedef struct 
{
	char* charbuff;		 /* circular buffer for phonetic input */
	int buffer_pos;			 /* Current position */
	int buffer_end;			 /* Last available phoneme */
	int buffer_size;		 /* number of chars in Phobuffer */
} Fifo;

#define charbuff(ff) ff->charbuff
#define buffer_pos(ff) ff->buffer_pos
#define buffer_end(ff) ff->buffer_end
#define buffer_size(ff) ff->buffer_size

int readline_Fifo(Fifo* ff, char *line, int size);
/* 
 * Read a line from the input stream in a circular buffer
 * Return 0 if there's nothing to read
 */

int write_Fifo(Fifo* ff, char *buffer_in);
/*
 * Write a string of phoneme in the input buffer
 * Return the number of chars actually written
 */

void reset_Fifo(Fifo* ff);
/*
 * Forget previously entered data in the circular buffer
 */

void  close_Fifo(Fifo* ff); 
/*
 * Release the memory
 */

Fifo* init_Fifo(int size);
/*
 * Constructor with size of the buffer
 */

#endif
