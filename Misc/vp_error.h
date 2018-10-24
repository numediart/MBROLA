/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    vp_error.h
 * Time-stamp: <01/09/26 16:39:44 mbrola>
 * Purpose: Errors management with debugging messages
 * Authors: V. Pagel and A. Ruelle
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
 * 10/06/96 : Created by VP. Don't be fool and don't rename it as "error.c"
 * unless you wish to introduce some confusion under UNIX platform with built
 * in error modules. Put here all little usefull functions to manage errors,
 * depending on the mode: LIBRARY, or CONSOLE (means stderr output or buffering
 * of the messages)
 *
 * Error messages as defined by A Ruelle
 *
 * 26/06/98: Back to C/ANSI. An inline is not possible any more for the usefull
 *           debug_message... consequence the call is still here in the
 *           generated code in non debug mode. This I do not wish -> MACROs 
 *           with size dependent names !
 */

#ifndef _VP_ERROR_H
#define _VP_ERROR_H

#include <stdio.h>
#include <stdarg.h>

/*
 * For the DLL and LIBRARY mode, Error codes returned
 */
#define ERROR_MEMORYOUT					-1
#define ERROR_UNKNOWNCOMMAND			-2
#define ERROR_SYNTAXERROR				-3
#define ERROR_COMMANDLINE          -4
#define ERROR_OUTFILE              -5
#define ERROR_RENAMING             -6
#define ERROR_NEXTDIPHONE          -7
 
#define ERROR_PRGWRONGVERSION		-10
 
#define ERROR_TOOMANYPITCH			-20
#define ERROR_TOOMANYPHOWOPITCH		-21
#define ERROR_PITCHTOOHIGH			-22
 
#define ERROR_PHOLENGTH				-30
#define ERROR_PHOREADING				-31
 
#define ERROR_DBNOTFOUND				-40
#define ERROR_DBWRONGVERSION			-41
#define ERROR_DBWRONGARCHITECTURE	-42
#define ERROR_DBNOSILENCE				-43
#define ERROR_INFOSTRING           -44
 
#define ERROR_BINNUMBERFORMAT			-60
#define ERROR_PERIODTOOLONG				-61
#define ERROR_SMOOTHING					-62
#define ERROR_UNKNOWNSEGMENT			-63
#define ERROR_CANTDUPLICATESEGMENT		-64
#define ERROR_TOOMANYPHONEMES                   -65

#define ERROR_BOOK		-70
#define ERROR_CODE		-71

#define WARNING_UPGRADE -80
#define WARNING_SATURATION -81

/* buffer cumulating error messages when in lib or dll mode */
extern char errbuffer[]; 
extern int lasterr_code;			  /* Code of the last error */

void fatal_message(const int code, const char *format, /* args */ ...);
/*
 * Uses the format of a printf function
 * throw an exception when in library mode, or abort the program
 */

void warning_message(const int code, const char *format, /* args */ ...);
/*
 * Uses the format of a printf function
 * Just print a warning in the error buffer
 */

#ifdef DEBUG
void debug_message(char const *format, /* args */ ...);

/* What's below is kind of ugly. When we're in C++ it can be replaced 
 *	by an inline debug_message function
 *
 * unavoidable if I don't want code to be generated in the release
 */
#define debug_message1(A)  debug_message(A) 
#define debug_message2(A,B)  debug_message(A,B) 
#define debug_message3(A,B,C)  debug_message(A,B,C) 
#define debug_message4(A,B,C,D)  debug_message(A,B,C,D) 
#define debug_message5(A,B,C,D,E) debug_message(A,B,C,D,E)
#define debug_message6(A,B,C,D,E,F) debug_message(A,B,C,D,E,F)
#define debug_message7(A,B,C,D,E,F,G) debug_message(A,B,C,D,E,F,G)
#define debug_message8(A,B,C,D,E,F,G,H) debug_message(A,B,C,D,E,F,G,H)

#else
/* don't generate anything */
#define debug_message1(A) 
#define debug_message2(A,B) 
#define debug_message3(A,B,C)
#define debug_message4(A,B,C,D) 
#define debug_message5(A,B,C,D,E) 
#define debug_message6(A,B,C,D,E,F) 
#define debug_message7(A,B,C,D,E,F,G) 
#define debug_message8(A,B,C,D,E,F,G,H) 

#endif

#endif
