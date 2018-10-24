/* FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    phonbuff.c
 * Purpose: Table of phonemes to implement a simple .pho parser
 *          Buffer of phonemes for pitch interpolation
 * Author:  Vincent Pagel 
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <00/03/29 14:36:10 pagel>
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
 * 11/06/98 : Created from readpho.c
 *            Warning -> this is a sample program of what a parser could
 *            be, feel free to implement your own parser (as long as it 
 *            follows the requirements of parser.h)
 *
 * 15/09/98 : In case there's too many phonemes without pitch points, we
 *  compulsorily add a value with default_pitch (avoid fatal_message)
 */
#include "common.h"
#include "diphone.h"
#include "database.h"
#include "hash_tab.h"
#include "mbrola.h"
#include "parser.h"
#include "phonbuff.h"

void init_FlushSymbol(PhoneBuff* pt, char *flush)
/* 
 * Build a new sscanf target to spot the flush symbol
 */
{
	/* Null means put a default value if there is none */
	if (flush==NULL)
		flush=FLUSH;
  
	if (flush_symbol(pt)!=NULL)
		MBR_free(flush_symbol(pt));
  
	flush_symbol(pt)= (char *) MBR_malloc( strlen(flush) + 3);
	sprintf(flush_symbol(pt),"%s%%n",flush);
}

void init_CommentSymbol(PhoneBuff* pt, char *comment)
/* 
 * Build a new sscanf target to spot the comment symbol
 */
{
	/* Null means put a default value if there is none */
	if (comment==NULL)
		comment=COMMENT;
	 
	if (comment_symbol(pt)!=NULL)
		free(comment_symbol(pt));
	 
	comment_symbol(pt)= (char *) MBR_malloc( strlen(comment) + 3);
	sprintf(comment_symbol(pt),"%s%%n",comment);
}

void initdummy_PhoneBuff(PhoneBuff* pt)
{
	Phone* my_phone;

	/* silence with dummy length, and  1 pitch point at 0% equal 
	 * to FirstPitch for interpolation
	 */
	my_phone=init_Phone(default_phon(pt), 0.0f);  
	appendf0_Phone(my_phone, 0.0f, default_pitch(pt));
	pt->Buff[0]=my_phone;

	CurPhone(pt)=0;    /* Forces FillCommandBuffer to read new data and  */
	NPhones(pt)=0;      /* consider PhoneBuff[0] as the previous phone   */
	state_pho(pt)=PHO_OK;  /* Reset ReadPho internal flag */
	Closed(pt)=False;
}

void free_residue_PhoneBuff(PhoneBuff *pt)
/* free allocated strings in the phonetable */
{
	int i;
  
	/* In charge of freeing what have not been passed to the synthesizer */
	for(i=CurPhone(pt); i<NPhones(pt)+1; i++)
		close_Phone(val_PhoneBuff(pt,i));	
}

void close_PhoneBuff(PhoneBuff *pt)
/* close the door before leaving */
{
	free_residue_PhoneBuff(pt);

	if (flush_symbol(pt))
    {
		MBR_free(flush_symbol(pt));
    }
	if (comment_symbol(pt))
    {
		MBR_free(comment_symbol(pt));
    }

	MBR_free(pt);
}

void reset_PhoneBuff(PhoneBuff *pt)
/* Before a synthesis sequence initialize the loop with a silence */
{
	/* Remove the reliquate */
	free_residue_PhoneBuff(pt);
	initdummy_PhoneBuff(pt);
	input(pt)->reset_Input(input(pt));
}

PhoneBuff* init_PhoneBuff(Input* my_input, char* default_phon, float default_pitch, float time_ratio, float freq_ratio, char* comment, char* flush)
/*
 * Phonetab Constructor
 */
{
	PhoneBuff* self= (PhoneBuff*) MBR_malloc( sizeof(PhoneBuff));

	input(self)=my_input;  /* polymorphic input stream */
  
	default_pitch(self)=default_pitch;
	default_phon(self)=default_phon;

	TimeRatio(self)=time_ratio;
	FreqRatio(self)=freq_ratio;

	initdummy_PhoneBuff(self);

	flush_symbol(self)=NULL;
	comment_symbol(self)=NULL;

	init_FlushSymbol(self,flush);
	init_CommentSymbol(self,comment);
  
	return(self);
}

void shift_PhoneBuff(PhoneBuff *pt)
/* 
 * Reset the phonetable to an empty value
 */
{
	/* PhoneBuff[0] filled with last phone of previous fill */
	head_PhoneBuff(pt)=tail_PhoneBuff(pt);
  
	/* The first pitch point will have index 1  */
	/* Leaves the first position free for 0% pitch point */
	CurPhone(pt)=0;
	NPhones(pt)=0;
	Closed(pt)=False;
}

void append_PhoneBuff(PhoneBuff *pt,char *name,float length)
/*
 * Append a new phone at the end of the table
 * if too many phones without pitch information, add a pitch point with
 * default pitch
 */
{
	Phone *my_phone;
  
	NPhones(pt)++;
	my_phone= init_Phone(name,length);
	tail_PhoneBuff(pt)= my_phone;

	/* Dummy point for later 0% value */
	appendf0_Phone(tail_PhoneBuff(pt), 0.0f, 0.0f);

	/* Test overflow ... I consider this is not fatal any more, just add a compulsory pitch point */
	if (NPhones(pt)==MAXNPHONESINONESHOT-2)
    {
		warning_message(ERROR_TOOMANYPHOWOPITCH,
						"Too many phones without pitch information at '%s'\n",name);
		
		/* Energic measures to force a pitch point */
		appendf0_Phone(my_phone, 0.0, default_pitch(pt));
    }
}

void interpolatef0_PhoneBuff(PhoneBuff *pt)
/*
 * Interpolate 0% and 100% value for each phone of the table
 */
{
	int i;
	float CurPos;
	float a, b;            /* Interpolation parameters */
	float InterpLength=0.0f;
  
	/* Sum the lengths without the borders */
	for(i=1; i<NPhones(pt); i++)
		InterpLength+= length_Phone( val_PhoneBuff(pt,i) );
  
	CurPos= length_Phone(head_PhoneBuff(pt)) - 
		pos_Pitch( tail_PitchPattern(head_PhoneBuff(pt)));
  
	/* Add the right and the left border for the interpolation */
	InterpLength+= pos_Pitch(val_PitchPattern(tail_PhoneBuff(pt),1)) + CurPos;
  
	/* From broad to narrow stylization : 
	 *     linear interpolation frequency=ax+b 
	 */
	if (InterpLength!=0)
    {
		/* Last pitch point of the first phoneme */
		b= freq_Pitch(tail_PitchPattern(head_PhoneBuff(pt)));

		/* First pitch point of the last phoneme */
		a= ( freq_Pitch(val_PitchPattern(tail_PhoneBuff(pt),1)) - b) / InterpLength ;

		/* Interpolate the core */
		for (i=1; i<NPhones(pt); i++)
		{
			/* reset the number of points */
			reset_Phone(val_PhoneBuff(pt,i));
			 
			/* Put pitch point at 0% */
			appendf0_Phone(val_PhoneBuff(pt,i), 0.0f, a*CurPos+b);
			 
			/* Put a pitch point at 100% */
			CurPos+= length_Phone(val_PhoneBuff(pt,i));
			appendf0_Phone(val_PhoneBuff(pt,i), 100.0f, a*CurPos+b);
		}
      
		/* Add 0% pitch on the last phoneme (minimal phone has 2 pitch points) */
		val_PhoneBuff(pt,NPhones(pt))->PitchPattern[0].pos=  0.0f;
		val_PhoneBuff(pt,NPhones(pt))->PitchPattern[0].freq= a*CurPos + b;
      
		/* Add 100% pitch point to the 1st phoneme */
		appendf0_Phone(head_PhoneBuff(pt),
					   100.0,
					   freq_Pitch(head_PitchPattern(val_PhoneBuff(pt,1))));
    }
  
	/* Interpolated, until next sequence ! */
	Closed(pt)=True;
}

StatePhone ReadLine(PhoneBuff* pt, char *line, int size)
/*
 * Skip empty lines
 * Skip comment line ( begins with a ; )
 * Detect # which means end of chunk ( equivallent to eof )
 * Return value may be: PHO_EOF,PHO_FLUSH,PHO_OK
 */
{
	unsigned int comment, stream_flush, leading_blank;
	float fvalue;
	char *rest;  
  
	debug_message1("ReadLine\n");
	do
    {
		/* 
		 * InputLine depends on what is connected to photab
		 * according to compilation mode
		 */
		if (! input(pt)->readline_Input( input(pt), line, size) ) 
			return(PHO_EOF);
		
		debug_message2("line: %s\n",
					   line);
		comment=0;
		stream_flush=0;
		leading_blank=0;
      
		sscanf(line," %n",&leading_blank);
		rest= &line[leading_blank];		
      
		sscanf(rest, comment_symbol(pt), &comment);
		sscanf(rest, flush_symbol(pt), &stream_flush);
      
		if (stream_flush)
			return(PHO_FLUSH);
      
		if	 (comment)
		{ 
			int command;
			 
			/* Check if this is a true comment ; or a command ;; */
			rest= &rest[comment];
			 
			command=0;
			sscanf(rest,comment_symbol(pt), &command); 
			 
			if (command)
			{
				/* Test ";; T=1.2"  <- Time ratio */
				/* Test ";; F=0.8"  <- Frequency ratio */
				/* Test ";; FLUSH ###"  <-  Flush symbol renaming */
				char new_name[100];
				  
				rest=&rest[comment];
				  
				if (sscanf(rest," T = %f",&fvalue)==1)
					TimeRatio(pt)=fvalue;
				else if (sscanf(rest," F = %f",&fvalue)==1)
					FreqRatio(pt)=fvalue;
				else if (sscanf(rest," FLUSH %s",new_name)==1)
				{  /* New Flush symbol */
					init_FlushSymbol(pt,new_name);
				}
			}
			else
			{ /* A true meaningless comment :-) */
			}
		}
    }
#if defined(TARGET_OS_DOS) || defined(__EMX__)
	while (comment || (leading_blank==strlen(line)) || (leading_blank==strlen(line)-1));
#else
	while (comment || (leading_blank==strlen(line)));
#endif
	debug_message1("done ReadLine\n");
	return(PHO_OK);
}

StatePhone FillCommandBuffer(PhoneBuff *pt)
/*
 * Reads phonemes from the input file and put it in a buffer for pitch 
 * interpolation.
 * Return value may be: PHO_EOF, PHO_FLUSH, PHO_OK
 *
 * Input file format is line with format :  Phoneme Length (pos freq)*
 * Phone = phoneme name
 * Length= phoneme length in ms
 * pos   = position (in % of total phone duration) of a pitch pattern point
 * freq  = F0 (Hz) value of that pitch pattern point
 */
{
	char name[255];
	float length;
	float pos, f0;
	int j,k;
	int  position,new_pos; /* string index of pitch-pair being processed */
	int val;	         /* Result of sscanf -> detection of syntax errors */
	StatePhone state_line;	 /* Return value */
	char a_line[1024]; /* An input line in the command file   */

	debug_message1("FillCommandBuffer\n");
  
	/* Order of the phoneme */
	j=1;
	do	  /* Analyze lines we have pitchpoints */
    {
		state_line=ReadLine(pt, a_line, sizeof(a_line));	   
      
		/* Incidents during reading */
#ifdef LIBRARY
		/* In LIBRARY or DLL mode */
		if (state_line==PHO_FLUSH)
#else
			/* in file mode -> eof on a file means FLUSH */
			if ( (state_line==PHO_FLUSH)
				 ||  (state_line==PHO_EOF))
#endif
			{
				/* If we have to flush, add 3 trailing silences */
				/* the 1st one will help reveal the last phoneme ! */
				append_PhoneBuff(pt, default_phon(pt),0);

				/* The 2nd will be issued */
				append_PhoneBuff(pt, default_phon(pt),0);

				/* 
				 * The 3rd one won't even issued with nextphone_Parser, but
				 * compulsory to provide F0 interpolation value
				 */
				append_PhoneBuff(pt, default_phon(pt),0);
				appendf0_Phone(tail_PhoneBuff(pt),0.0f,default_pitch(pt)*FreqRatio(pt));
				break;
			}
			else if (state_line==PHO_EOF)
			{
				/* If EOF, then simply return the state as is for later completion */
				return(PHO_EOF);
			}
		
		/* Retrieves PHONEME_NAME LENGTH_IN_MS  */
		
		val=sscanf(a_line,"%s %f%n",name,&length,&position);

		/* it's a silence add an anti spreading 0ms silence */
		if (strcmp(name, default_phon(pt))==0)
		{
			append_PhoneBuff(pt, default_phon(pt),0); 
		}
		
		/* A New phoneme */
		append_PhoneBuff(pt, name, length*TimeRatio(pt));
		
		if (val!=2)		/* Check syntax */
		{
			fatal_message(ERROR_SYNTAXERROR,"Fatal error in line:%s\n",a_line);
			return PHO_ERROR;
		}
      
		/*
		 * Read pairs of POSITION PITCH_VALUE till the end of a_line
		 * Eventually the pairs can be surrounded with ()
		 * WARNING:if the first condition is checked, the second call to sscanf
		 *         is dropped
		 */
		k=1;
		while ( (sscanf(&a_line[position]," ( %f , %f ) %n", &pos, &f0, &new_pos)==2)
				|| (sscanf(&a_line[position]," %f %f%n", &pos, &f0, &new_pos)==2))
		{
			f0*=FreqRatio(pt);
			appendf0_Phone(tail_PhoneBuff(pt),pos,f0);
			k++;
			position+=new_pos;
		}
      
		/* Check for residual characters in the line */
		if (sscanf(&a_line[position],"%*s")!=-1)
		{ 
			fatal_message(ERROR_UNKNOWNCOMMAND,
						  "Fatal error in line:%s\n"
						  "At the pitch pair:%s????\n",
						  a_line,&a_line[position]);
			return PHO_ERROR;
		}
				
		j++;
    } while ( NPitchPatternPoints(tail_PhoneBuff(pt)) == 1 );
  
	/* We have a serie of phonemes with coherent pitch points, 
	 * interpolate them, and close the sequence 
	 */
	interpolatef0_PhoneBuff(pt);

#ifdef DEBUG
	{
		int jj;

		debug_message1("READLINE:\n");
		for(jj=0;jj<=NPhones(pt);jj++)
		{
			int kk;
			debug_message2("Phoneme:%s ",
						   name_Phone(val_PhoneBuff(pt,jj)));
			for(kk=0; kk< NPitchPatternPoints(pt->Buff[jj]) ; kk++)
				debug_message3("(%f %f) ",	
							   pos_Pitch(val_PitchPattern(pt->Buff[jj],kk)),
							   freq_Pitch(val_PitchPattern(pt->Buff[jj],kk)));
			debug_message1("\n");
		}
	}
	debug_message1("done FillCommandBuffer\n");
#endif
	return(state_line);
}

StatePhone next_PhoneBuff(PhoneBuff *pt, Phone** ph)
/*
 * Reads a phone from the phonetic command buffer and prepares the next
 * diphone to synthesize ( prev_diph )
 * Return value may be: PHO_EOF,PHO_FLUSH,PHO_OK
 *
 * NB : Uses only phones from 1 to ... NPhones-1 in the buffer.
 * Phone 0 = memory from previous buffer.
 */
{
	debug_message1("NextDiphone\n");

	if ((CurPhone(pt)==NPhones(pt))
		|| (!Closed(pt)))
    { /* Must read buffer ahead */
      
      /* Shift the phonetable if new data needed */
		if (CurPhone(pt)==NPhones(pt))
			shift_PhoneBuff(pt);
		
		/*
		 * Test if the previous call to FillCommandBuffer had resulted in
		 * FLUSH or EOF.   EOF is possible only with the non-library version
		 */
		if ( (state_pho(pt)==PHO_FLUSH) ||
			 (state_pho(pt)==PHO_EOF))
		{
			StatePhone temp=state_pho(pt);
			state_pho(pt)=PHO_OK; /* reset */
			return(temp);
		}
      
		/* Read new phoneme serie */
		state_pho(pt)=FillCommandBuffer(pt);

		/* Premature exit */
		if (state_pho(pt)==PHO_ERROR)
			return PHO_ERROR;
		
		/* The pitch sequence is not complete */
		if (!Closed(pt))
		{
			state_pho(pt)=PHO_OK; /* reset for further readings */
			return(PHO_EOF);
		}
    }
  
	*ph= val_PhoneBuff(pt,CurPhone(pt)) ;
	CurPhone(pt)++;
	debug_message1("done NextDiphone\n");
	return(PHO_OK);
}
