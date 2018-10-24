/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:  wrapper.c
 * Time-stamp: <2000-03-28 18:02:19 pagel>
 * Purpose: Wrapper to build a all-in-one library version of the 
 *          one-channel mode
 * Authors: Pagel Vincent
 * Email : mbrola@tcts.fpms.ac.be
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
 * 10/01/97: Created
 *    Used to hide mbrola file structure from the outside when distributing 
 *    lib
 *
 * 27/03/99: Romdatabases
 */

#undef MULTI_CHANNEL
#define LIBRARY

#include "../Misc/common.h"
#include "../Misc/vp_error.h"
#include "../Misc/mbralloc.h"
#include "../Misc/incdll.h"
#include "../Parser/phone.h"
#include "../Database/diphone_info.h"
#include "../Engine/diphone.h"
#include "../Engine/mbrola.h"
#include "../Database/database.h"
#include "../Database/database_old.h"
#include "../Misc/audio.h"
#include "../Parser/parser.h"
#include "../Parser/parser_input.h"
#include "../Database/hash_tab.h"
#include "../LibOneChannel/onechannel.h"

#include "../Misc/mbralloc.c"
#include "../Database/little_big.c"
#include "../Misc/common.c"
#include "../Parser/phone.c"
#include "../Engine/diphone.c"
#include "../Misc/g711.c"
#include "../Misc/audio.c"
#include "../Engine/mbrola.c"
#include "../Database/diphone_info.c"
#include "../Database/database_old.c"
#include "../Database/database.c"
#include "../Database/zstring_list.c"
#include "../Parser/phonbuff.c"
#include "../Parser/fifo.c"
#include "../Parser/parser_input.c"
#include "../Parser/input_fifo.c"
#include "../Parser/input_file.c"
#include "../Database/hash_tab.c"
#include "../LibOneChannel/onechannel.c"
#include "../Misc/vp_error.c"

#ifdef BACON
#include "../Database/database_bacon.c"
#endif

#ifdef CEBAB
#include "../Database/database_cebab.c"
#endif

#if defined(ROMDATABASE_STORE) || defined(ROMDATABASE_INIT)
#include "../Database/rom_database.c"
#include "../Database/rom_handling.c"
#endif
