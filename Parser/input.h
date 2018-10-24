/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    input.h
 * Purpose: polymorphic type for input stream
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

#ifndef _INPUT_H
#define _INPUT_H

typedef struct Input Input;

typedef int (*readline_InputFunction)(Input* in, char *line, int size);
typedef void (*close_InputFunction)(Input* in);
typedef void (*reset_InputFunction)(Input* in);

struct Input
{
	void* self;
	readline_InputFunction readline_Input;
	close_InputFunction close_Input;
	close_InputFunction reset_Input;
};

#endif
