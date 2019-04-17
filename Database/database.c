/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    database.c
 * Time-stamp: <01/09/26 16:45:04 mbrola>
 * Purpose: diphone database management
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
 * 29/02/96 : Created. Put here everything concerning the speech database 
 *
 * 02/02/97 : Replacement diphones enabled (T. Dutoit)
 *
 * 08/09/97 : New format -> phonemes are written as Zstring (old limit in 
 *            size was 2 char/phoneme)
 *            New policy for silence definition
 *
 * 20/10/97 : Database information = Zstrings at the end of the dba
 *
 * 05/11/97 : BACON info for 2.05c
 *
 * 25/02/98 : Database formats cleaning
 *            drop old compatibility checks with 2.02 and so on
 *            Escape sequence in database_info for non printable information
 *            archiving the database construction parameters 
 *
 * 03/03/98 : Non posix platforms can't realloc on a NULL pointer
 *
 * 23/06/98 : Polymorphic databases !
 *            A cloning mechanism for multi-instanciation databases
 *
 * 28/08/98 : in my way to C/ANSI, dropped the 'const' definition
 *            (that would require extern variables :-(
 *            Thus "const FrameType VOICING_MASK=2;"
 *            becomes "#define VOICING_MASK 2"
 *
 * 20/03/00 : keep tracks of the longest diphone in the database to
 *            remove static limits from mbrola engine
 *
 *            Detects when opening directories .... it's a FAQ because error 
 *            message was not clear, adding a Database extension like ".dba"
 *            should do
 *
 *            Rom databases included (database can be FILE* or int16* depending on
 *            the database mode)
 *            
 *            25% extra space in the hashtable enhances search
 *     
 *            Pitchmark in memory are kept compressed (4 in on byte)
 */
#include "common.h"
#include "little_big.h"
#include "database.h"
#include "database_old.h"

#ifdef BACON
#include "database_bacon.h"
#endif

#ifdef CEBAB
#include "database_cebab.h"
#endif

#ifndef ROMDATABASE_PURE 
/*
 * THE FOLLOWING FUNCTIONS ARE USED FOR DATABASES ON FILE !!!!
 */

/* Table of database constructors */
init_DatabaseFunction init_tab[ ]= { init_DatabaseOld,   /* type 0 */
									 init_DatabaseBasic, /* type 1 */
# ifdef BACON
									 init_DatabaseBacon, /* type 2 */
# else
									 init_DatabaseBasic,
#endif
#ifdef CEBAB
									 init_DatabaseCebab, /* type 3 */
#   else
									 init_DatabaseBasic,
#   endif
};

#define NB_DATABASE_TYPE (sizeof(init_tab)/sizeof(init_DatabaseFunction))
#define void_(x) if (x) {}

char* ReadDatabaseZstring(FILE* database)
/* 
 * Read a zstring in the database (allocate the memory)
 */
{
	int c;
	char* PhoneName=NULL;
	int index=0;
  
	do
    {
		c=fgetc(database);
      
		/* End of file means end of string :-) */
		if (c==EOF)
			c=0;
		index+=1;
      
		if(PhoneName != NULL) 
			PhoneName=(char *) MBR_realloc(PhoneName,index);
		else 
			PhoneName=(char *) MBR_malloc(index);
      
		PhoneName[index-1]= (char) c;
    }
	while (c!=0);
	return PhoneName;
}

bool ReadDatabaseInfo(Database* dba)
/*
 * Extract textual information from the database if any
 */
{
	/* go to the 'end' of the file and read the information */
	fseek(database(dba),RawOffset(dba)+SizeRaw(dba),SEEK_SET);
	while (!feof(database(dba)))
    {
		PhonemeName str= ReadDatabaseZstring( database(dba) );
		append_ZStringList( info(dba), str );
		MBR_free(str);
    }
	return True;
}

/*
 * Three parts of the Database header
 */

bool ReadDatabaseHeader(Database* dba)
/* 
 * Reads the diphone database header , and initialize variables 
 */
{
	FILE* database;
	uint16 OldSizeMrk;			  /* TO DO: Destroy when it becomes outdated !! */
	int i;
  
	debug_message1("Read Header\n");
  
	/*
	 * Load the diphone database
	 */
	if ((database=fopen(dbaname(dba),"rb"))==NULL)
    {
		fatal_message(ERROR_DBNOTFOUND,"FATAL ERROR : cannot find file %s !\n",dbaname(dba));
		return False;
    }
  
	dba->database = (void*) database;
  
	/* Sanity check -> big endian waiting on the corner */
	i= fread(&Magic(dba),1,6,database); /* Reads "MBROLA" */
	if (i==0)  
    {
		fatal_message(ERROR_DBWRONGVERSION,
					  "Database format error\n"
					  "%s is an empty file (could be a directory)\n"
					  ,dbaname(dba));
    }
  
	if ( (i != 6) 
		 || (strncmp("MBROLA",(char *)Magic(dba),6))
		 || (Magic(dba)[0] != MAGIC_HEADER))
    {
		fatal_message(ERROR_DBWRONGVERSION,
					  "Binary number format error\n"
					  "You are probably using a version of %s incompatible\n"
					  "with your machine architecture.\n"
					  "Get the right one from the MBROLA project homepage :\n"
					  "   " WWW_ADDRESS "\n"
					  ,dbaname(dba));
		return False;
    }
  
	for (i=0;i<5;i++) 
		Version(dba)[i]= (char) fgetc(database);
	Version(dba)[5]='\0';
  
	readl_int16(&nb_diphone(dba),database); 
  
	/*
	 * Version hack due to limited representation to keep compatibility 
	 * between formats  TO DO : Remove when outdated ! 
	 */
	readl_uint16(&OldSizeMrk,database);
	if (OldSizeMrk==0)		 
    { /* Sign of the new format !!! */
		readl_int32(&SizeMrk(dba),database);
    }
	else
    { /* old format */
		SizeMrk(dba)= OldSizeMrk;
    }
  
	readl_int32(&SizeRaw(dba), database);
	readl_int16(&Freq(dba), database);
  
	void_(fread(&MBRPeriod(dba), sizeof(MBRPeriod(dba)), 1, database));
	void_(fread(&Coding(dba), sizeof(Coding(dba)), 1, database));
  
	if (strcmp( Version(dba), SYNTH_VERSION) > 0 )
    {
		fatal_message(ERROR_DBWRONGARCHITECTURE,
					  "This program is not compatible with your database\n"
					  "Get the last version of mbrola at www address :\n" 
					  "    " WWW_ADDRESS "\n");
		return False;
    }
  
	if ( MBRPeriod(dba) > 400 )
    {
		warning_message(ERROR_PERIODTOOLONG,
						"Warning: Period %i is really long\n",
						MBRPeriod(dba));
    }
  
	if (strcmp("2.05",Version(dba))>0)
    {	/* Oh my, we're older than 2.05, let's call the compatibility driver! */
		Coding(dba)=0;
    }
  
	debug_message2("VERSION %s\n", Version(dba));
	debug_message2("nb_diphone %i\n",nb_diphone(dba));
	debug_message2("SizeMrk %i\n",SizeMrk(dba));
	debug_message2("SizeRaw %i\n",SizeRaw(dba));
	debug_message2("Freq %i\n",Freq(dba));
	debug_message2("MBRPeriod %i\n",MBRPeriod(dba));
	debug_message2("Coding %i\n",Coding(dba));
	return True;
}

bool ReadDatabaseIndex(Database* dba)
/* 
 * Read the index table of diphones, and put them in the hash table
 */
{
	FILE* database=database(dba);
	int error=False;
	int i;

	int32 indice_pm=0;   /* cumulated pos in pitch mark vector */
	int32 indice_wav=0;  /* cumulated pos in the waveform dba */
	uint8 nb_wframe;     /* Physical number of frame */

	PhonemeName new_left;
	PhonemeName new_right;
	int16 new_halfseg;
	uint8 new_nb_frame;
	int32 new_pos_pm;
	int32 new_pos_wave;

	/* initialize hash table: add extra 25% to enhance performances */
	diphone_table(dba)=init_HashTab( nb_diphone(dba) + nb_diphone(dba)/4);
  
	/* Insert diphones one by one in the hash table */
	for(i=0;  (indice_pm!=SizeMrk(dba))&&(i<nb_diphone(dba)); i++)
    {
		/* Read data from file */
		new_left= ReadDatabaseZstring(database);
		new_right= ReadDatabaseZstring(database);
		readl_int16(& new_halfseg, database);
        void_(fread(&new_nb_frame, sizeof(new_nb_frame), 1, database));
		void_(fread(&nb_wframe, sizeof(nb_wframe), 1, database));
      
		new_pos_pm= indice_pm;
		indice_pm+= new_nb_frame;
      
		if (indice_pm==SizeMrk(dba))
		{
			/* The last diphone of the database is _-_  */
			sil_phon(dba)= MBR_strdup(new_left);
			debug_message2("Init silence with %s\n", new_left);
		}
		
#if defined(BACON) || defined(CEBAB)
		/* Well it should be in a Bacon/Cebab dependent file but
		   for the moment it's possible to factorize :-) */
		if (Coding(dba)==DIPHONE_RAW)
		{
			new_pos_wave= indice_wav;
			indice_wav+= (long) nb_wframe * (long) MBRPeriod(dba);
		}
		else
		{ /* Extra data for BACON or CEBAB coder  */
			readl_int32( &new_pos_wave, database);
		}
#else
		new_pos_wave= indice_wav;
		indice_wav+= (long) nb_wframe* (long) MBRPeriod(dba);
#endif
		debug_message8("%i Diph %s-%s  poswav=%li halfseg=%li  pospm=%i nbframe=%i\n",
					   i, new_left, new_right, 
					   new_pos_wave, new_halfseg, new_pos_pm, new_nb_frame );
      
		add_HashTab(diphone_table(dba), new_left, new_right, 
					new_pos_wave, new_halfseg, new_pos_pm, new_nb_frame );

		MBR_free(new_left);
		MBR_free(new_right);

		/* Keep a record of the longest diphone (to allocate oversized
		 * temporary buffers in DiphoneSynthesis)
		 */
		if (nb_wframe > max_frame(dba))
		{ max_frame(dba)=nb_wframe; }
    }
  
	/* dutoit 02/02/97 
	 * Check if we've reached the end of the diphone table
	 * The rest are replacement diphones, they don't correspond
	 * to new samples, but duplicate existing diphones
	 */
	for( ; i<nb_diphone(dba); i++)
    {
		int position;
		PhonemeName left= ReadDatabaseZstring(database);
		PhonemeName right= ReadDatabaseZstring(database);
      
		position= search_HashTab(diphone_table(dba), left, right);
      
		/* Sanity check: the target exist */
		if (position==NONE)
		{
			error=True;
			warning_message(ERROR_CANTDUPLICATESEGMENT,
							"Fatal error: Can't duplicate %s-%s segment\n",
							left, 
							right);
		}
		else
		{
			/* Clone the original */
			DiphoneInfo* one_cell= content(diphone_table(dba), position);;
	  
			debug_message4("Copy %s-%s position %i ",
						   left,
						   right,
						   position);
	  
			/* Was usefull to search in the database, but not any more */
			MBR_free(left);
			MBR_free(right);
	  
			/* Duplicate info!! and set the new name */
			left = ReadDatabaseZstring(database);
			right= ReadDatabaseZstring(database);
	  
			debug_message3("into %s-%s \n",
						   left,
						   right);
	  
			/* Sanity check: the target allready exist */
			position= search_HashTab(diphone_table(dba), left, right);
			if (position!=NONE)
			{
				error=True;
				warning_message(ERROR_CANTDUPLICATESEGMENT,
								"Fatal error: duplicate %s-%s segment allready exist\n",
								left,
								right);
			}
	
			add_HashTab(diphone_table(dba), left, right, 
						pos_wave(*one_cell),
						halfseg(*one_cell), 
						pos_pm(*one_cell),
						nb_frame(*one_cell) );

			/* Was usefull to search in the database, but not any more */
			MBR_free(left);
			MBR_free(right);
		}
    }
  
	if (error)
    {
		fatal_message(ERROR_CANTDUPLICATESEGMENT,
					  "Fatal error: No recovery\n");
		return False;
    }
  
#ifdef DEBUG_HASH
	tuning_HashTab(diphone_table(dba));
#endif
 
	/* Size of the buffer that will be allocated in Diphonesynthesis */
	max_samples(dba)= MBRPeriod(dba) * max_frame(dba);

	return True;
}


bool ReadDatabasePitchMark(Database* dba)
/*
 * Load pitch markers (Voiced/Unvoiced, Transitory/Stationnary)
 */
{
	FILE* database=database(dba);
	int32 round_size= (SizeMrk(dba)+3)/4;  /* round to upper value */
  
	/* Compress 4 pitch marks in one byte */
	pmrk(dba)= (FrameType *) MBR_malloc( round_size *sizeof(FrameType) );
	void_(fread( pmrk(dba), sizeof(FrameType), round_size, database));
  
	RawOffset(dba)=ftell(database);
	debug_message1("done Pitchmark\n");
	return True;
}

/*
 * Initialisation and loading of Diphones -> depend on database Coding
 */

void close_DatabaseBasic(Database* dba)
/*
 * Release the memory for a raw database
 */
{ 
	debug_message2("close_DatabaseBasic : %i\n", Coding(dba));
  
	if ( info(dba) )
		close_ZStringList( info(dba) );   /* close ReadDatabaseInfo */
  
	close_HashTab( diphone_table(dba) ); /* close ReadDatabaseIndex */
  
	if (pmrk(dba))
		MBR_free(pmrk(dba));  /* close ReadDatabasePitchMark */
  
	if (database(dba))
		fclose(database(dba));  /* close ReadDatabaseHeader */
  
	if (dbaname(dba))
		MBR_free(dbaname(dba));  /* file name */
  
	if (sil_phon(dba))
		MBR_free(sil_phon(dba)); /* silence phoneme, string */
  
	MBR_free(dba);
	debug_message1("done close_DatabaseBasic\n");
}

Database* init_DatabaseBasic(Database* dba)
/* 
 * Basic version, read raw waves = Check there's no coding 
 * Returning NULL means error
 */
{
	debug_message1("init_DatabaseBasic\n");

	/* init virtuals */
	dba->getdiphone_Database= getdiphone_DatabaseBasic;
	dba->close_Database= close_DatabaseBasic;
  
	/* A basic dba contains RAW waveforms */
	if ( Coding(dba) != DIPHONE_RAW )
    {
		dba->close_Database(dba);
		fatal_message( ERROR_BINNUMBERFORMAT,
					   "This program can't decode your database\n"
					   "Get the last version of mbrola at www address :\n"
					   "    " WWW_ADDRESS "\n");
		return NULL;
    }
  
	if ( ! ReadDatabaseIndex(dba)     ||
		 ! ReadDatabasePitchMark(dba) ||
		 ! ReadDatabaseInfo(dba) )
    {
		dba->close_Database(dba);
		return NULL;
    }
  
	debug_message1("done init_DatabaseBasic\n");
	return dba;
}


Database* init_Database(char* dbaname)			  
/* Generic initialization, calls the appropriate constructor 
 * Returning NULL means fatal error (check LastErr)
 */
{
	Database* mydba;
  
	debug_message1("init_Database\n");
  
	mydba= (Database*) MBR_malloc(sizeof(Database));
  
	/* First initialize the name, and give 0 values for error handling and 
	 * premature exit 
	 */
	dbaname(mydba)=  MBR_strdup(dbaname);
	mydba->database= NULL;
	sil_phon(mydba)= NULL;
	max_frame(mydba)=  0;
	pmrk(mydba)= NULL;
	diphone_table(mydba)= NULL;
	mydba->close_Database= close_DatabaseBasic; /* will be changed depending on the dba type */
	info(mydba)= init_ZStringList();
  
	if (! ReadDatabaseHeader(mydba) )
    {
		mydba->close_Database(mydba);
		return NULL;
    }
  
	if ( Coding(mydba) > NB_DATABASE_TYPE )
    {
		mydba->close_Database(mydba);
		fatal_message( ERROR_BINNUMBERFORMAT,
					   "This program can't decode your database\n"
					   "Get the last version of mbrola at www address :\n"
					   "    " WWW_ADDRESS "\n");
		return NULL;
    }
  
	debug_message1("virtual init_Database\n");
  
	/* Yes it is that simple !! Watch your step */
	return init_tab[Coding(mydba)](mydba);
}

Database* init_rename_Database(char* dbaname, ZStringList* rename, ZStringList* clone)
/* 
 * A variant of init_Database allowing phoneme renaming on the fly 
 * 
 * rename and clone can be NULL to indicate there's nothing to change
 *
 * Renaming is a ONCE consuming operation (the database is changed
 * at loading) -> it involves a complete reconstruction of the hash table
 * but nothing else at run-time
 */
{
	Database* mydba= init_Database(dbaname);
	PhonemeName new_sil= NULL;
  
	debug_message1("init_rename_Database\n");
  
	/* Standard initialization failed ? */
	if (!mydba)
		return NULL;
  
	/* Test if renaming apply on the silence diphone */
	if (rename)
		new_sil= find_rename_ZStringList(rename, sil_phon(mydba));
  
	if (new_sil)
    {
		MBR_free(sil_phon(mydba));
		sil_phon(mydba)=MBR_strdup(new_sil);
    }
  
	if (rename)
		diphone_table(mydba)= diphone_rename_HashTab( diphone_table(mydba), rename);

	if (clone)
		diphone_table(mydba)= diphone_clone_HashTab( diphone_table(mydba), clone);
  
	debug_message1("done Init_rename_Database\n");
	return mydba;
}

#endif /* ROMDATABASE_PURE .... next function can be used in both worlds */



int getDatabaseInfo(Database* dba, char* msg, int size, int index)
/*
 * Retrieve the ith info message, NULL means get the size
 */ 
{
	int return_size=0;	  /* Default error flag */
  
	if ( (index < size_ZStringList(info(dba)))  && 
		 (index >= 0) )
    {
		/* request the size of an information message */
		if (msg==NULL)
			return_size= strlen( decode_ZStringList( info(dba), index) )  + 1;
		else
		{ /* request the message itself */
			strncpy(msg, decode_ZStringList( info(dba), index), size);
			msg[size-1]= 0; /* append 0 for security if msg is > size */
			return_size= strlen(msg); /* effective size */
	  
			/* 
			 * Non printable information archiving the database construction
			 * parameters 
			 */
			if (msg[0] == (char) INFO_ESCAPE)
				msg[0]= 0;
		}
    }
	return return_size;
}

bool getdiphone_DatabaseBasic(Database* dba, DiphoneSynthesis *diph)
/* 
 * Basic loading of the diphone specified by diph. Stores the samples
 * Return False in case of error
 */
{
	debug_message2("getDiphone_DatabaseBasic : %i \n",
				   Coding(dba));
	debug_message3(" diphone  : %s-%s ", 
				   name_Phone(LeftPhone(diph)),
				   name_Phone(RightPhone(diph)));

	if (! init_common_Database(dba,diph))
		return False;
  
	debug_message3(" Nbframe : %d,    add : %ld ",
				   nb_frame_diphone(diph), 
				   pos_wave_diphone(diph));
  
#ifdef ROMDATABASE_INIT
	if ( Coding(dba)& ROM_MASK)
    { /* 
       * We're in ROM, it's as simple as that 
       */
		buffer(diph)= (int16*) rom_wave_ptr(dba) + pos_wave_diphone(diph);
    }
	else
#endif /* ROMDATABASE_INIT */
    { /* 
       * We're on file 
       */
#ifndef ROMDATABASE_PURE

		fseek(database(dba), 
			  pos_wave_diphone(diph) * sizeof(int16) + RawOffset(dba),
			  SEEK_SET);
      
		/* Sanity check */
		if ( tot_frame(diph) > max_frame(dba) )
		{
			fatal_message(ERROR_PHOLENGTH,
						  "PANIC: phone %s-%s -> %i frames > Max=%i\n",
						  name_Phone(LeftPhone(diph)),
						  name_Phone(RightPhone(diph)),
						  tot_frame(diph), max_frame(dba));
			return False;
		}
      
		if ( readl_int16buffer( buffer(diph), tot_frame(diph)*MBRPeriod(dba), database(dba)) 
			 != (unsigned) tot_frame(diph)*MBRPeriod(dba) )
		{
			fatal_message(ERROR_PHOREADING,
						  "PANIC when reading phone %s-%s\n",
						  name_Phone(LeftPhone(diph)), 
						  name_Phone(RightPhone(diph)));
			return False;
		}
#endif /* ROMDATABASE_PURE */
    }
  
	debug_message1("done getdiphone_DatabaseBasic\n");
	return True;
}


void init_real_frame(DiphoneSynthesis *diph)
/*
 * Make the link between logical and physical frames
 */
{
	int pred_type;	        /* Type ( Voiced/ Unvoiced ) of the previous frame */
	uint8 i;         		  /* physical number of frames of the diphone */
  
	/* Stride thru the pitch marks to spot Unvoiced-Voiced transitions */
	tot_frame(diph)=1;
	real_frame(diph)[0]=1;
	pred_type=V_REG;
  
	for(i=1; i<= nb_frame_diphone(diph) ; tot_frame(diph)++,i++)
    {
		/* Check for extra frame at the end of an unvoiced sequence */
		if ( !(pred_type & VOICING_MASK )
			 && 
			 ( pmrk_DiphoneSynthesis(diph, i) & VOICING_MASK ))
		{
			tot_frame(diph)++;
		}
      
		real_frame(diph)[i]=tot_frame(diph);
		pred_type=pmrk_DiphoneSynthesis(diph, i);
    }
	tot_frame(diph)--;
  
	/* If the last is unvoiced, bonus frame !!! */
	if (! (pred_type & VOICING_MASK ))
		tot_frame(diph)++;
}

bool init_common_Database(Database* dba, DiphoneSynthesis *diph)
/*
 * Common initialization shared among all database types
 */
{
	/* Search the diphone index in the database */
	int i= search_HashTab(diphone_table(dba), 
						  name_Phone(LeftPhone(diph)),
						  name_Phone(RightPhone(diph)));
  
	if (i==NONE)
		return False;

	Descriptor(diph)= content(diphone_table(dba),i);  
  
	/* Substract 1 as index will go from 1 to N (instead of 0..N-1) */
	diph->p_pmrk= & (pmrk(dba)[ diph->Descriptor->pos_pm / 4 ]);
	diph->p_pmrk_offset= diph->Descriptor->pos_pm % 4;
  
	/* link between physical and logical frames */
	init_real_frame(diph);

	return True;
}

#ifdef MULTI_CHANNEL

static void close_DatabaseCopy(Database* dba)
/*
 * Close a database created with the copyconstructor
 */
{
	debug_message1("close_DatabaseCopy\n");
  
#ifdef ROMDATABASE_INIT
	if ( Coding(dba) & ROM_MASK )
    {
		/* Nothing to close if it's a ROM database */
    }
	else
#endif /* ROMDATABASE_INIT */
    {
#ifndef ROMDATABASE_PURE
		if ( database(dba) )
			fclose( database(dba) );
#endif
    }
  
	/* don't release anything else as it may be shared ! */
	MBR_free(dba);
}

Database* copyconstructor_Database(Database* old_dba)
/* Creates a copy of a diphone database so that many synthesis engine 
 * can use the same database at the same time (duplicate the file handler
 * and connect a dumb "close_Database"
 *
 * Highly recommended with multichannel mbrola, unless you can guaranty
 * mutually exclusive access to the getdiphone function
 */
{
	Database* mydba;
  
	debug_message1("copyconstructor_Database\n");
  
	mydba= (Database*) MBR_malloc(sizeof(Database));
  
	/* clone ! */
	*mydba= *old_dba;
  
	/* Closing should not release shared object */
	mydba->close_Database= close_DatabaseCopy;
  
	/* duplicate the file handler ... if any */
#ifdef ROMDATABASE_INIT
	if ( Coding(mydba)&ROM_MASK )
    {
		/* Nothing to duplicate if it's a ROM database */
    }
	else
#endif
    {
#ifndef ROMDATABASE_PURE
		if (( mydba->database= (void*) fopen(dbaname(mydba),"rb")) == NULL)
		{
			mydba->close_Database(mydba);
			fatal_message(ERROR_DBNOTFOUND,
						  "FATAL ERROR : cannot find file %s !\n",
						  dbaname(mydba));
			return NULL;
		}
#endif
    }
  
	return mydba;
}

#endif /* MULTI_CHANNEL MODE */
