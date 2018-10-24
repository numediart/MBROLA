/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    synth.h
 * Purpose: Main function of the MBROLA speech synthesizer.
 * Authors: Vincent Pagel
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
 */

#ifndef _SYNTH_H
#define _SYNTH_H

#include "common.h"

/* 
 * In standalone mode, input and ouput through files
 */
extern FILE *command_file; /* File providing the phonetic input (can be stdin) */ 
extern FILE *output_file;  /* Audio output file (can be stdout)  */

/* used in standalone compilation mode */
extern int main(int argc, char **argv);

#endif
