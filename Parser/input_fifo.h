/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    input_file.h
 * Purpose: input stream from  a fifo (instanciation of input.h)
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

#ifndef _INPUT_FIFO_H
#define _INPUT_FIFO_H

#include "input.h"
#include "fifo.h"

Input* init_InputFifo(Fifo* my_fifo);

#endif

