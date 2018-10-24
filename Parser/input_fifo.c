/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    input_fifo.c
 * Purpose: input stream from a fifo (instanciation of input.h)
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
 * 22/06/98 : Created
 */


#include "fifo.h"
#include "input.h"
#include "common.h"

/* TODO TODO Alleviate the writing
   directly call with self for all functions, except Close
*/

static int readline_InputFifo(Input* in, char *line, int size)
{
	return( readline_Fifo((Fifo*) in->self,line,size) );
}
  
static void reset_InputFifo(Input* in)
{
	reset_Fifo((Fifo*) in->self);
}

static void close_InputFifo(Input* in)
{
	MBR_free(in);
}
 
Input* init_InputFifo(Fifo* my_fifo)
{
	Input* self= (Input*) MBR_malloc( sizeof(Input) );

	self->self= (void*) my_fifo;
	self->readline_Input= readline_InputFifo;
	self->close_Input= close_InputFifo;
	self->reset_Input= reset_InputFifo;

	return self;
}
