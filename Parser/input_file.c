/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    input_file.c
 * Purpose: input stream from a file (instanciation of input.h)
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

#include "mbralloc.h"
#include "input_file.h"


/* TODO TODO Alleviate the writing
   directly call with self for all functions, except Close
*/

static long readline_InputFile(Input* in, char *line, int size)
{
	return( (long) fgets(line, size, (FILE*)in->self));
}

static void reset_InputFile(Input* in)
/* nothing to reset a file ! */
{
}

static void close_InputFile(Input* in)
/* nothing to close */
{
	MBR_free(in);
}

Input* init_InputFile(FILE* my_file )
{ 
	Input* self= (Input*) MBR_malloc( sizeof(Input) );

	self->self= (void*) my_file;
	self->readline_Input= readline_InputFile;
	self->close_Input= close_InputFile;
	self->reset_Input= reset_InputFile;

	return self;
}
