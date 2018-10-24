/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    fifo.c
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

#include "fifo.h"
#include "common.h"

int readline_Fifo(Fifo* ff, char *line, int size)
/* 
 * Read a line from the circular input buffer
 * Return 0 if there's nothing to read
 */
{
	int i=0;
	char last=0;
  
	while ( (i<size-1) && 
			(last!=LINE_FEED) &&
			(buffer_pos(ff)!=buffer_end(ff)))
	{
		last=line[i]=charbuff(ff)[buffer_pos(ff)];
		buffer_pos(ff)++;
		i++;
		
		/* Circular buffer */
		if (buffer_pos(ff)==buffer_size(ff))
			buffer_pos(ff)=0;
	}
	line[i]=0;
	return(i);
}

int write_Fifo(Fifo* ff, char *buffer_in)
/*
 * Write a string of phoneme in the input buffer
 * Return the number of chars actually written
 */
{
	int i=0;
	int available=buffer_pos(ff)-buffer_end(ff);
  
	if (available<=0)
		available+= buffer_size(ff);
  
	/* Fail to write */
	if ( (int) strlen(buffer_in) >= available)
		return(0);
  
	while (buffer_in[i]!=0)
	{
		/* Check overflow */
		if (buffer_end(ff)==buffer_pos(ff)-1)
			return(i);
		
		charbuff(ff)[buffer_end(ff)]=buffer_in[i];
		i++;
		buffer_end(ff)++;
		
		/* Circular buffer */
		if (buffer_end(ff)==buffer_size(ff))
			buffer_end(ff)=0;
	}
	return(i);
}

void reset_Fifo(Fifo* ff)
/*
 * Forget previously entered data in the circular buffer
 */
{
	buffer_pos(ff)=0;
	buffer_end(ff)=0;
}

void close_Fifo(Fifo* ff)
/*
 * Close the door before leaving
 */
{
	MBR_free(charbuff(ff));
	MBR_free(ff);
}

Fifo* init_Fifo(int size)
/*
 * Constructor with size of the buffer
 */
{
	Fifo* self=(Fifo*) MBR_malloc( sizeof(Fifo) );
	charbuff(self)= (char*) MBR_malloc(size);
	buffer_size(self)=size;
	reset_Fifo(self);
	return(self);
}
