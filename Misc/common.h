/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    common.h
 * Time-stamp: <01/09/26 16:37:48 mbrola>
 * Purpose: common utilities and defines
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
 */

/******************
 * Definitions    *
 ******************/

#ifndef _COMMON_H
#define _COMMON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Visual C++ doesn't have POSIX header out of box */
#ifndef _MSC_VER
#include <unistd.h>
#endif

/* The Dll version is based on the library one -> export DLL functions */
#ifdef DLL
# include "windows.h"
# define LIBRARY
# define DLL_EXPORT WINAPI
#else
# define DLL_EXPORT
#endif

/* 
 * Sanity check on compilation mode!
 */
#if defined(ROMDATABASE_PURE) && defined(ROMDATABASE_STORE)
# error "ROMDATABASE_PURE and ROMDATABASE_STORE are incompatible"
#endif
#if defined(ROMDATABASE_PURE) && !defined(ROMDATABASE_INIT)
# error "ROMDATABASE_PURE without ROMDATABASE_INIT: hey pal, are you going to work?"
#endif

/*
 * On VMS, '-' can't be used as a file name, use '_' instead
 * Also use a hand-made version of swab
 */
#ifdef TARGET_OS_VMS
#define OPENRT "r"
#define PIPESYMB "_"
#else
#define OPENRT "rt"
#define PIPESYMB "-"
#endif

/* For beboxes and Mac use a hand-made version of swab */
#if defined(TARGET_OS_VMS) || defined(TARGET_OS_BEOS) || defined(__STRICT_ANSI__)
#if ! defined(__MINGW64__) && ! defined(__MINGW32__) && ! defined(TARGET_OS_MAC)
void swab( const char *from, char *to, int nbytes);
#endif
#endif

#if defined(TARGET_OS_MAC)
#define swab(a, b, c) memcpy(a, b, c)
#endif

#if defined(__GLIBC__)
#include <endian.h>
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#if __BYTE_ORDER == __LITTLE_ENDIAN
#  define LITTLE_ENDIAN
#else
#  define BIG_ENDIAN
#endif
#else

/* Intel based machine ? */
#if defined(__i386) || defined(_M_X86) || defined(TARGET_OS_VMS)
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
# define LITTLE_ENDIAN
#endif

/* Endianess __sparc for SUN */
#if defined(__powerpc__) || defined(__sparc)
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#define BIG_ENDIAN
#endif

#ifdef __powerpc__
#include <endian.h>  /* Make sure we see the definitions */
#endif
#endif

/* memmove is not defined on SUN4, but memcpy will do */
#ifdef TARGET_OS_SUN4
#define memmove memcpy
#endif

#ifndef TARGET_OS_DOS
#include <unistd.h> 
#else
/* swab for Windows */
#define swab(X,Y,Z) _swab(X,Y,Z) 
#endif

#include "mbralloc.h"
#include "vp_error.h"
#include "incdll.h"

/* Release number (automatically changed by "make version") */
#define SYNTH_VERSION "3.4-dev"
#define WWW_ADDRESS "https://github.com/numediart/MBROLA/"
#define DB_ADDRESS "https://github.com/numediart/MBROLA-voices/"

/* General trace       */
/*	#define DEBUG           */

/* Trace of the hash table -> this debug make the program stop and
 * and print the access statistics in the hash table (may help to
 * check and tune access time on new databases)
 */
/* #define DEBUG_HASH      */

/*
 * True and False should be used instead of integer values
 */
/* Argh ! Depends on the compiler! Comment it if yours is not C/ANSI */
#define bool int
#define False 0
#define True 1

/*
 * The Flush pseudo phoneme is used to force the flushing of the audio output
 * when one uses pipes or library mode.
 */
#define FLUSH "#"

/*
 * First character of a comment line in the phonetic input
 */
#define COMMENT ";"

/* If the OS is able to catch signals ( well I mean Unix platforms ) */
#ifdef SIGNAL
#include <signal.h>
extern volatile sig_atomic_t  must_flush;  /* To catch the users signal (means flush the input) */
#endif


/*
 * ARCHITECTURE DEPENDENT !!!
 * These definitions should be imposed so that int8, int16 and int32
 * always refer to 8, 16 and 32 bits integer
 *
 * WARNING int16 and int32 are signed, uint16 and uint8 are NOT signed
 */

#define uint8  unsigned char
#define int8   char
#define int16  short
#define uint16 unsigned short
#define int32  int

/* The name as a string */
typedef char* PhonemeName;

/* Phoneme name encoded according to auxiliary table */
typedef uint16 PhonemeCode;
#define MAX_PHONEME_NUMBER 65000

#endif
