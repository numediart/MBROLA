/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    parser_fifo.c
 * Purpose: polymorphic type to parse a "pho file"
 * Time-stamp: <2000-03-06 12:32:30 pagel>
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

#ifndef _PARSER_H
#define _PARSER_H

#include "phone_export.h"

/* Return values of the nextphone function */
typedef enum {
	PHO_OK,
	PHO_EOF,
	PHO_FLUSH,
	PHO_ERROR
} StatePhone;


/* Polymorphic type */
typedef struct Parser Parser;

typedef void (*reset_ParserFunction)(Parser* ps);
typedef void (*close_ParserFunction)(Parser* ps);
typedef StatePhone (*nextphone_ParserFunction)(Parser* ps,Phone** ph);

/* 
 * Generic parser :
 *   reset: forget remaining data in the buffer (when the user STOP synthesis for example 
 *
 *   close: release the memory
 *
 *   nextphone: return the next Phoneme from input.
 *
 *     PRECONDITION: this phoneme MUST have a pitch point at 0 and 100%
 *
 *     THE CALLER IS IN CHARGE OF CALLING close_Phone ON THE PHONES HE GETS
 *     WITH nextphone
 */

struct Parser
{
	void* self;                /* Polymorphic on the real type */
	reset_ParserFunction reset_Parser;         /* virtual func */
	close_ParserFunction close_Parser;         /* virtual func */
	nextphone_ParserFunction nextphone_Parser; /* virtual func */
};

#endif
