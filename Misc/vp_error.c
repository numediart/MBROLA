/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    vp_error.c
 * Time-stamp: <2000-03-28 17:58:44 pagel>
 * Purpose:  Errors management and debugging messages
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
 *           with size dependent names :-(
 */

#include "common.h"
#include "vp_error.h"

/* buffer cumulating error messages when in lib or dll mode */
char errbuffer[300];	
int lasterr_code;				  /* Code of the last error */

void fatal_message(const int code, const char *format, /* args */ ...)
/*
 * Uses the format of a printf function
 * throw an exception when in library mode, or abort the program
 */
{
	va_list ap;
  
	va_start(ap,format);
	lasterr_code=code;
  
#ifdef LIBRARY
	vsprintf(errbuffer, format, ap);
	va_end(ap);
#else
	vfprintf(stderr, format, ap);
	va_end(ap);
	exit(code); /* in standalone mode... brutal */
#endif
}

void warning_message(const int code, const char *format, /* args */ ...)
/*
 * Uses the format of a printf function
 * Just print a warning in the error buffer
 */
{
	va_list ap;
  
	va_start(ap,format);
	lasterr_code=code;
  
#ifdef LIBRARY
	vsprintf(errbuffer, format, ap);
	va_end(ap);
#else
	vfprintf(stderr, format, ap);
	va_end(ap);
#endif 
}

#ifdef DEBUG

void debug_message(char const *format, /* args */ ...)
/* 
 * Uses the format of a printf function
 * Previously was inline for C++, but no solution in ANSI C
 * We don't want that the Debug message generate any code in the release 
 * version of Mbrola -> this is the reason for the inline declaration as
 * when we're not debugging the macro is empty, and no code is generated
 */
{	
	va_list ap;
  
	va_start(ap,format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fflush(stderr); /* critical if the program crashes */
}

#endif
