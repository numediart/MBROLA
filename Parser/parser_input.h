/* FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    parser_input.h
 * Purpose: parse a "pho file" from a polymorphic input stream
 *          Instanciation of parser.h
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <1998-10-21 16:16:34 pagel>
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
 * 21/10/98 : Initialize flush
 */

#ifndef PARSER_INPUT_H
#define PARSER_INPUT_H

#include "parser.h"
#include "input.h"

Parser* init_ParserInput(Input* my_input, char* silence, float pitch, float time_ratio, float freq_ratio, char* comment, char* flush);
/*
 * Constructor of the parser. Need to know initial default pitch, and
 * initial default phoneme as well
 */

#endif
