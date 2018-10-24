/*
 * File:    mbrola.c
 * Purpose: Diphone-based MBROLA speech synthesizer.
 * Authors:  Thierry Dutoit and Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 * Time-stamp: <00/04/11 13:59:18 pagel>
 *
 * FPMs-TCTS SOFTWARE LIBRARY
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
 * This file contains an implementation or Belgian patent BE09600524
 * and US Patent Nr. 5,987,413                                                                                    
 *
 * 10/01/97 : Created. 
 *
 * 22/10/97 : incredible bug with maxnconcat
 *
 * 17/06/98 : Mbrola is now pseudo object code with a This pointer
 *
 * 31/08/98 : init ola_win to 0 for PCs
 *            bug for 0ms long phonemes
 *
 * 04/09/98 : return error code on every floor (to replace catch/throw)
 *
 * 08/09/98 : correct double memory free in case of reset_Mbrola
 *            correct leftPhone and rightPhone affectation order for cur_diph
 *            to avoid memory leak in case of premature ending
 *
 * 18/05/99 : decrement zero_padding() and add a condition not to overlap
 *            with phonemes having 0ms length
 *
 * 10/06/99 : ola_win is zeroed in reset_Mbrola instead of init_Mbrola 
 *            (clean buffer in case of playback interruption)
 *
 * 05/09/99 : boundcheck on "first" in Concat
 *
 * 28/03/00 : Rom databases, malloc'ed diphone buffers
 *            Test in Concat for degenerated case "0ms long phonemes"
 */

#include <math.h>

#include "common.h"
#include "diphone.h"
#include "database.h"
#include "audio.h"
#include "hash_tab.h"
#include "parser.h"
#include "mbrola.h"

void init_Hanning(float* table,int size,float ratio)
/* 
 * Initialize the Hanning weighting window  
 * Ratio is used for volume control (Volume is embedded in hanning to
 * spare a multiplication later ...)
 */
{
	int i;
  
	for (i=0; i<size; i++) 
		table[i]= (float) (ratio * 0.5 * (1.0 - cos((double)i*2*3.14159265358979323846/(float)size)));
}

void set_voicefreq_Mbrola(Mbrola* mb, uint16 OutFreq)
/* Change the Output Freq and VoiceRatio to change the vocal tract 	*/
{
	VoiceFreq(mb)=OutFreq;
	VoiceRatio(mb)=(float) VoiceFreq(mb) / (float)Freq(diph_dba(mb));
}

uint16 get_voicefreq_Mbrola(Mbrola* mb)
/* Get output Frequency */
{ return( VoiceFreq(mb)); }

void set_smoothing_Mbrola(Mbrola* mb, bool smoothing)
/* Spectral smoothing or not */
{ smoothing(mb)= smoothing; }

bool get_smoothing_Mbrola(Mbrola* mb)
/* Spectral smoothing or not */
{ return smoothing(mb) ; }

void set_no_error_Mbrola(Mbrola* mb, bool no_error)
/* Tolerance to missing diphones */
{ no_error(mb)=no_error; }

bool get_no_error_Mbrola(Mbrola* mb)
/* Spectral smoothing or not */
{ return no_error(mb); }

void set_volume_ratio_Mbrola(Mbrola* mb, float volume_ratio)
/* Overall volume */
{ 
	init_Hanning( weight(mb), 2*MBRPeriod(diph_dba(mb)), volume_ratio);
	volume_ratio(mb)= volume_ratio;
}

float get_volume_ratio_Mbrola(Mbrola* mb)
/* Overall volume */
{ return volume_ratio(mb); }

void set_parser_Mbrola(Mbrola* mb, Parser* parser)
/* drop the current parser for a new one */
{ parser(mb)= parser; }

Mbrola* init_Mbrola(Database* dba)
/* 
 * Connect the database to the synthesis engine, then initialize internal 
 * variables. Connect the phonemic command stream later with set_parser_Mbrola
 */
{
	Mbrola* mb;
	debug_message1("init_Mbrola\n");

	mb= (Mbrola*) MBR_malloc(sizeof(Mbrola));
	diph_dba(mb) =dba;

	/* Allocate buffers */
	ola_win(mb) = MBR_malloc( sizeof(float)* MBRPeriod(dba)* 2 );
	ola_integer(mb) = MBR_malloc( sizeof(int16)* MBRPeriod(dba)*2 );
	weight(mb) = MBR_malloc( sizeof(float)* MBRPeriod(dba)*2 );
  
	/* Default settings ! */
	set_voicefreq_Mbrola(mb, Freq(dba)); /* VoiceRatio=1.0 */
	set_volume_ratio_Mbrola(mb, 1.0f);
	set_smoothing_Mbrola(mb,True);
	set_no_error_Mbrola(mb,False);

	saturation(mb) =False;
	audio_length(mb) =0;
	last_time_crumb(mb) =0;
#ifdef LIBRARY
	first_call(mb)=True;
#endif

	/* prev_diph points to the previous diphone synthesis structure
	 * and cur_diph points to the current one. While progressing to 
	 * the next diphone, prev_diph memory is resetted the pointers
	 * are swapped between cur and prev diphones
	 */
  
	prev_diph(mb)= init_DiphoneSynthesis(MBRPeriod(dba), 
										 max_frame(dba),
										 max_samples(dba) );

	cur_diph(mb)=  init_DiphoneSynthesis(MBRPeriod(dba), 
										 max_frame(dba),
										 max_samples(dba) );

	nb_end(mb)=1000; /* set to high value for first pass in Concat() */
	debug_message1("done init_Mbrola\n");
	return(mb);
}

void close_Mbrola(Mbrola* mb)
/* Free the memory ! */
{
	debug_message1("close_Mbrola\n");
	/*
	 * Right phoneme of prev_diph, and Left phoneme of cur_diph are shared references
	 * So to avoid double desallocation, scratch one (cur_diph may be scratched in 
	 * case of error)
	 */
	LeftPhone(cur_diph(mb))=NULL;

	close_DiphoneSynthesis(cur_diph(mb));
	close_DiphoneSynthesis(prev_diph(mb));							 

	/* Buffers and windows */
	MBR_free( ola_win(mb) );
	MBR_free( ola_integer(mb) );
	MBR_free( weight(mb) );

	MBR_free(mb);
	debug_message1("done close_Mbrola\n");
}

bool reset_Mbrola(Mbrola* mb)
/* 
 * Gives initial values to current_diphone (not synthesized anyway)
 * -> it will give a first value for prev_diph when we make the first
 *    NextDiphone call so that cur_diph= _-FirstPhon with lenght1=0
 *    and prev_diph= _-_ with length2=0
 *
 * return False in case of error
 */
{
	int i;
  
	debug_message1("reset_Mbrola\n");

	/* Free residual phonemes
	 * Right phoneme of prev_diph, and Left phoneme of cur_diph are shared.
	 * So to avoid double desallocation, scratch one 
	 */
	reset_DiphoneSynthesis(cur_diph(mb));
	RightPhone(prev_diph(mb))=NULL;
	reset_DiphoneSynthesis(prev_diph(mb));
  
	/* Set dummy cur_diphone with empty value */
	LeftPhone(cur_diph(mb)) = init_Phone( sil_phon(diph_dba(mb)), 0.0); 
	RightPhone(cur_diph(mb)) = init_Phone( sil_phon(diph_dba(mb)), 0.0); 
  
	if (!diph_dba(mb)->getdiphone_Database( diph_dba(mb), cur_diph(mb) ))
    {
		fatal_message( ERROR_DBNOSILENCE,
					   "_-_ PANIC with %s!\n",
					   sil_phon(diph_dba(mb)));
		return False;
    }
  
	/* Indicate we don't generate anything with the first _-_ diphone  */
	nb_pm(cur_diph(mb))=1;
  
	/* The ola window is null from the start */
	for (i=0; i< 2*MBRPeriod(diph_dba(mb)); i++)
		ola_win(mb)[i]=0.0f;
  
#ifdef LIBRARY
	/* Indicate that the first call to read_MBR must trigger an initialization */
	first_call(mb)=True;
#endif
  
	debug_message1("done reset_Mbrola\n");
	return True;
}

StatePhone NextDiphone(Mbrola* mb)
/*
 * Reads a phone from the phonetic command buffer and prepares the next
 * diphone to synthesize ( prev_diph )
 * Return value may be: PHO_EOF,PHO_FLUSH,PHO_OK, PHO_ERROR
 */
{
	int len_left, len_right, tot_len;
	float wanted_len;
	int len_anal;	   /* Number of sample in last segment analysed     */
	Phone* my_phone;
	StatePhone state;
  
	DiphoneSynthesis *temp;
  
	debug_message1("NextDiphone\n");
  
	if ((state=parser(mb)->nextphone_Parser(parser(mb),&my_phone))!=PHO_OK)
    {
		debug_message1("done NextDiphone PHO_NOT_OK\n");
		return(state);
    }
  
	/* length and freq modified if the vocal tract length is not 1.0 */
	applyRatio_Phone(my_phone,VoiceRatio(mb));
  
	/* Prev_diph has been used ... release LeftPhone 
	 * don't RightPhone, as reference on it is still active in cur_diph 
	 */
	if (LeftPhone(prev_diph(mb)))
    {
		close_Phone(LeftPhone(prev_diph(mb)));   /* once used, release the phoneme */
		LeftPhone(prev_diph(mb))=NULL;
    }
  
	/* Shift the diphone being worked on */
	/* prev_diph is the one that will be synthesized */
	temp=prev_diph(mb);	
	prev_diph(mb)=cur_diph(mb); 
	cur_diph(mb)=temp;
  
	/* Initialize minimal info of the new cur_diph */
	RightPhone(cur_diph(mb))= my_phone;
	LeftPhone(cur_diph(mb)) = RightPhone(prev_diph(mb)); /* Reuse common part ! */
  
	/* 
	 * Load the diphone from the database !!
	 */
	if ( ! diph_dba(mb)->getdiphone_Database( diph_dba(mb), cur_diph(mb)) )
    {
		bool success= False; /* then ... we failed */
      
		/* Try to arrange a solution for the missing diphone */
		if (no_error(mb)) 
		{
			PhonemeName temp_left= name_Phone( LeftPhone( cur_diph(mb)));
			PhonemeName temp_right= name_Phone( RightPhone( cur_diph(mb)));
	  
			warning_message(ERROR_UNKNOWNSEGMENT,
							"Warning: %s-%s unkown, replaced with _-_\n", 
							name_Phone(LeftPhone( cur_diph(mb))),
							name_Phone(RightPhone( cur_diph(mb))));
	  
			/* Momentary replacement with _-_ */
			name_Phone( LeftPhone( cur_diph(mb)))= sil_phon( diph_dba(mb));
			name_Phone( RightPhone( cur_diph(mb)))= sil_phon(diph_dba(mb));
	  
			success= diph_dba(mb)->getdiphone_Database( diph_dba(mb), cur_diph(mb));
	  
			/* Restore situation */
			name_Phone( LeftPhone( cur_diph(mb)))= temp_left;
			name_Phone( RightPhone( cur_diph(mb)))= temp_right;
		}
      
		if (!success)
      	{ 
			fatal_message(ERROR_UNKNOWNSEGMENT,
						  "Fatal error: Unkown recovery for %s-%s segment\n",
						  name_Phone(LeftPhone(cur_diph(mb))),
						  name_Phone(RightPhone(cur_diph(mb))));
	  
			return PHO_ERROR;
		}
    }
  
	/*
	 * Computation of length of 2nd part of prev_diph and of 1st part of current
	 * one : current phoneme gives current diphone => impose length of 2nd
	 * part of previous and of 1st part of current one proportionally to their
	 * relative importance in the database.
	 * Ex : phoneme sequence = A B C. When synthesizer receives C, B-C is
	 * scheduled, and the length of B in A-B and B-C  are derived. Synthesis of
	 * A-B can then start.
	 */
  
	len_anal = ( nb_frame_diphone(prev_diph(mb)) ) * MBRPeriod(diph_dba(mb));
	len_left= len_anal -  halfseg_diphone(prev_diph(mb));
	len_right= halfseg_diphone(cur_diph(mb)) ;
	tot_len=len_left+len_right;
  
	wanted_len= length_Phone( LeftPhone(cur_diph(mb)) );
  
	Length2(prev_diph(mb))= (int) (wanted_len * (float)len_left * (float)Freq(diph_dba(mb))
								   / 1000.0f / (float)tot_len) ;
	Length1(cur_diph(mb)) = (int) (wanted_len * (float) Freq(diph_dba(mb)) / 1000.0f - 
								   Length2(prev_diph(mb))) ;

	debug_message3("done NextDiphone PHO_OK -> %s-%s\n",
				   name_Phone(RightPhone(prev_diph(mb))),
				   name_Phone(my_phone));
	return(PHO_OK);
}

bool MatchProsody(Mbrola* mb)
/*
 * Selects Duplication or Elimination for each analysis OLA frames of
 * the diphone we must synthesize (prev_diph). Selected frames must fit
 * with wanted pitch pattern and phonemes duration of prev_diph
 *
 * Return False in case of error
 */
{
	int nb_frame;             /* Nbr of pitch markers in the database segment*/
	float theta1;             /* Time warping coefficients               */
	float theta2=0.0f;
	float beta=0.0f;
	int halfseg;              /* Index (in samples) of center of diphone     */
	int cur_sample;           /* sample offset in synthesis window           */
	int t;                    /* time in analysis window                     */
	int limit;                /* sample where to stop                        */
	int i;
	int k;                    /* dummy variable !!!                          */
	int len_anal;             /* Number of sample in last segment analysed   */
	int start;
	int old_len1;             /* Length1 in samples before adjustment */
  
	debug_message1("MatchProsody\n");
  
	/* Modify the length of the 2nd part of a phone if the end of its 1st 
	   part does not correspond to its theoretical value */
	start=(int) ( length_Phone(LeftPhone(prev_diph(mb))) - 
				  Length1(prev_diph(mb)) / (Freq(diph_dba(mb))*1000.0f));
	old_len1=Length1(prev_diph(mb));
    
	if (old_len1>0)
    {
		Length1(prev_diph(mb))+= last_time_crumb(mb) ;

		/* redraw pitch curve with new length */
		for (i=0; i< NPitchPatternPoints(LeftPhone(prev_diph(mb))) ;i++)
			if (PitchPattern(LeftPhone(prev_diph(mb)))[i].pos > start)
			{
				PitchPattern(LeftPhone(prev_diph(mb)))[i].pos= start+
					(PitchPattern(LeftPhone(prev_diph(mb)))[i].pos - start) * 
					Length1(prev_diph(mb))/old_len1;
			}
    }

	nb_frame = nb_frame_diphone(prev_diph(mb));
	halfseg = halfseg_diphone(prev_diph(mb));
	len_anal = nb_frame * MBRPeriod(diph_dba(mb));

	theta1 = ((float) Length1(prev_diph(mb))) / (float) halfseg;
	if (len_anal-halfseg!=0)
    {
		theta2 = ((float) Length2(prev_diph(mb))) / (float) (len_anal-halfseg);
		beta   = (float) Length1(prev_diph(mb)) - (theta2*halfseg);
    }

	/*
	 * Assign to each synthesis frame the corresponding one in segment analysis
	 */
	frame_number(mb)[0]=1;
	frame_pos(mb)[0]=0;

	k=1;   /* first window pos (sample)   */
	debug_message3("matchpro %i= %i\n",nb_frame, pmrk_DiphoneSynthesis( prev_diph(mb), 1));
  
	if ( pmrk_DiphoneSynthesis( prev_diph(mb), 1)
		 & VOICING_MASK )
    {
		cur_sample = GetPitchPeriod(prev_diph(mb), 0, Freq(diph_dba(mb)));
    }
	else 
    {
		cur_sample = MBRPeriod(diph_dba(mb));
    }
  
	t= MBRPeriod(diph_dba(mb));
  
	/* stride through analysis frames  */
	for(i=1; i<=nb_frame ;i++)
    {
		if (t <= halfseg)	/* Left phone */
			limit = (int) (t*theta1);
		else		/* Right phone */
			limit = (int) (t*theta2 + beta);
      
		/* Stride through synthesis windows */
		while (cur_sample <= limit)
		{
			frame_number(mb)[k]=i;    /* NB : first frame number = 1 */
			frame_pos(mb)[k]= cur_sample;
			k++;

			debug_message3("match %i= %i\n", i, pmrk_DiphoneSynthesis( prev_diph(mb), i));

			if (pmrk_DiphoneSynthesis( prev_diph(mb), i)
				& VOICING_MASK )
			{
				cur_sample += GetPitchPeriod(prev_diph(mb),cur_sample,Freq(diph_dba(mb)));
			}
			else
			{
				cur_sample += MBRPeriod(diph_dba(mb));
			}
			if (k>=NBRE_PM_MAX)
			{
				fatal_message( ERROR_PITCHTOOHIGH,
							   "%s-%s Concat : PANIC, check your pitch :-)\n",
							   name_Phone(LeftPhone(prev_diph(mb))),	
							   name_Phone(RightPhone(prev_diph(mb))));
				return False;
			}
		}
		t=t+MBRPeriod(diph_dba(mb));
    }
 
	frame_number(mb)[k]=0; /* End tag */
	nb_pm(prev_diph(mb)) = k-1; /* Number of pitchmarks for synthesis */
  
	/* total length that should have been synthesized -last effective sample */
	last_time_crumb(mb)+= (old_len1+Length2(prev_diph(mb))) - frame_pos(mb)[k-1] ;

	debug_message1("done MatchProsody\n");
	return True;
}

void Concat(Mbrola* mb)
/*
 * This is a unique feature of MBROLA.
 * Smoothes diphones around their concatenation point by making the left
 * part fade into the right one and conversely. This is possible because
 * MBROLA frames have the same length everywhere.
 *
 * output : nb_begin, nb_end -> number of stable voiced frames to be used
 * for interpolation at the end of Leftphone(prev_diph(mb)) and the beginning
 * of RightPhone(prev_diph(mb)).
 */
{
	int c;
	int begin, end; /* Nbr of voiced frames at begining and end of prev_diph(mb)*/
	int first,last;	       /* Number of the first frame */
	int last_frame, first_frame; /* offset in sample for the last and first  */
	/* frame of concatenation point             */
	int16 *buff_left;            /* speech buffer on left of junction  */
	int16 *buff_right;           /* speech buffer on right of junction */
	int i,j;
	int cur_sample;	         /* sample offset in synthesis window        */
	int maxnconcat;
	int limitframe;
  
	debug_message1("Concat\n");

	/* We compute the first Olaed frame for cur_diph -> we can't do
	 * Matchprosody on it yet since we lack pitch points...
	 * But at least we have a pitch point at 0 for cur_diph
	 */
	cur_sample = GetPitchPeriod(cur_diph(mb), 0, Freq(diph_dba(mb)));

	if (Length1(cur_diph(mb))!=0)
    {
		float theta= ((float) Length1(cur_diph(mb))) / halfseg_diphone(cur_diph(mb));
      
		/* Use CEIL as first belongs to [1..N] */
		first= (int) ceil((double)cur_sample / MBRPeriod(diph_dba(mb)) / theta);
    }
	else
    {
		first=0;	  /* 0 length phonemes */
    }
  
	if (first > frame_number(mb)[ nb_pm(cur_diph(mb)) ])
    {
		first= frame_number(mb)[ nb_pm(cur_diph(mb)) ];
    }
  
	last= frame_number(mb)[nb_pm(prev_diph(mb))];
  
	if ( 
		(first!=0) &&   /* Degenerated case= phoneme with 0ms length */ 
		(pmrk_DiphoneSynthesis( prev_diph(mb), last ) & VOICING_MASK) && 
		(pmrk_DiphoneSynthesis( cur_diph(mb),  first )  & VOICING_MASK))
    { 
		/* Difference vector beetween LAST and FIRST Voiced frame */
		smooth(cur_diph(mb))=True;

		/* buffer goes from 0 to NB_SAMPLE while real_frame goes from 1 to N */
		last_frame = MBRPeriod(diph_dba(mb)) * ( real_frame(prev_diph(mb))[last] -1 );
		buff_left= &buffer(prev_diph(mb))[last_frame];
      
		first_frame = MBRPeriod(diph_dba(mb)) * (real_frame(cur_diph(mb))[first] -1 );
		buff_right= &buffer(cur_diph(mb))[first_frame];
		
		/* For the first half, no problem */
		for(i=0;i<MBRPeriod(diph_dba(mb));i++)
			smoothw(cur_diph(mb))[i] = (int) ((buff_left[i]-buff_right[i]) * weight(mb)[i]);
	 
		/* For the second half, reset counters of looped frames */
		for(j=0;i<(MBRPeriod(diph_dba(mb))*2); i++,j++)
			smoothw(cur_diph(mb))[i] = (int) ((buff_left[j]-buff_right[j]) * weight(mb)[i]);
    }
	else
		smooth(cur_diph(mb))=False;

	/*
	 * Count voiced frames on prev_diph(mb) ( available for smoothing )
	 * 'begin' for the left part and 'end' for the right part
	 */
	limitframe= halfseg_diphone(prev_diph(mb)) / MBRPeriod(diph_dba(mb));
	maxnconcat=(int) (0.5*nb_pm(prev_diph(mb))+1.0);
	if (maxnconcat>6) maxnconcat=6;
	
	begin=1;
	while ((pmrk_DiphoneSynthesis(prev_diph(mb), frame_number(mb)[begin]) == V_REG)
		   && (frame_number(mb)[begin] <= limitframe) 
		   && (begin < maxnconcat))
		begin++;
  
	/* if VREG VREG ... VREG VTRA : smooth VTRA too */
	if ( ( begin>1 )
		 && ( pmrk_DiphoneSynthesis(prev_diph(mb),frame_number(mb)[begin]) == V_TRA)
		 && ( frame_number(mb)[begin] <= limitframe)
		 && ( begin < maxnconcat))
		begin++;
  
	nb_begin(mb) = begin -1;			    
	/* Check previous value of nb_end */
	if (nb_begin(mb) > nb_end(mb)) 
		nb_begin(mb)=nb_end(mb);

	debug_message2("nb_begin %i \n", nb_begin(mb));

	end=1;
	c=nb_pm(prev_diph(mb))+1;
	while ((pmrk_DiphoneSynthesis(prev_diph(mb),frame_number(mb)[c-end]) == V_REG) 
		   && (frame_number(mb)[c-end] > limitframe) 
		   && (end < maxnconcat))
		end++;

	/* if VTRA VREG VREG ... VREG : smooth VTRA too */
	if ((end > 1) 
		&& (pmrk_DiphoneSynthesis(prev_diph(mb),frame_number(mb)[c-end]) == V_TRA)
		&& (frame_number(mb)[c-end] > limitframe) 
		&& (end < maxnconcat))
		end++;
	nb_end(mb) = end - 1;

	/* simplification: just check 1st frame of cur_diph, if not
	 * voiced, no interpolation at end of prev_diph(mb)
	 */
	if (pmrk_DiphoneSynthesis(cur_diph(mb), 1) != V_REG) 
		nb_end(mb)=0; 

	debug_message2("end %i ", nb_end(mb));
	debug_message1("done Concat\n");
}

void FlushFile(Mbrola* mb, int shift, int shift_zero)
/*
 * Flush on file what's computed
 */
{
	int k;
  
	for (k=0;k<shift;k++) 
    {
		if (ola_win(mb)[k] > 32765)
		{
			saturation(mb)=True;
			ola_integer(mb)[k]=32765;
		}
		else if (ola_win(mb)[k] < -32765)
		{
			saturation(mb)=True;
			ola_integer(mb)[k]=-32765;
		}
		else 
			ola_integer(mb)[k]= (int16) ola_win(mb)[k];
    }
  
	/* Amount that has been flushed */
	buffer_shift(mb)=shift;

	/* Zero padding ! */
	zero_padding(mb)= shift_zero;  
    
#ifndef LIBRARY
	audio_length(mb)+= write_int16s(ola_integer(mb), shift, output_file); 
  
	/* Fill the gap between 2 frames for extra low pitch */
	/* No lower limit, but write MBRPeriod buffer each time */
	if (shift_zero>0)
    {		 
		int shift_mod;						  /* Modulo for shift zero */
		int written;
		
		if (shift_zero>2*MBRPeriod(diph_dba(mb)))
			shift_mod=2*MBRPeriod(diph_dba(mb));
		else
			shift_mod=shift_zero;
		
		for (k=0; k<shift_mod; k++) ola_integer(mb)[k]=0;
		while (shift_zero>2*MBRPeriod(diph_dba(mb)))
		{
			written= write_int16s(ola_integer(mb), 2*MBRPeriod(diph_dba(mb)), output_file);
			buffer_shift(mb)+=written;
			audio_length(mb)+=written;
			shift_zero-=written;
		}
		written= write_int16s(ola_integer(mb),shift_zero, output_file);
		audio_length(mb)+=written;
		buffer_shift(mb)+=written;
    }
#endif
}

void OverLapAdd(Mbrola* mb, int frame)
/*
 *  OLA routine
 */
{
	int k;
	float correction;	        /* Energy correction factor */
	int end_window, add_window;
	int lim_smooth;		/* Beyond this limit -> left smoothing */
	float tmp;
	FrameType type;			/* Frame type */
	int shift_zero;		/* Noman's land between 2 ola filled with 0 */
	int shift;			/* Shift between pulses */
  
	debug_message1("OverLapAdd\n");

	shift = frame_pos(mb)[frame]-frame_pos(mb)[frame-1];
	if ((correction = (float)shift/(float)MBRPeriod(diph_dba(mb)))>=1) correction=1.0f;
  
	/* Keep nothing of previous frames as there's no overlap */
	shift_zero= shift - 2*MBRPeriod(diph_dba(mb));
	if (shift_zero>0)
		shift= 2*MBRPeriod(diph_dba(mb));
  
	end_window = 2*MBRPeriod(diph_dba(mb)) - shift;
	add_window =MBRPeriod(diph_dba(mb)) * (real_frame(prev_diph(mb))[frame_number(mb)[frame]]-1);
	lim_smooth = nb_pm(prev_diph(mb))-nb_end(mb);

	/* Flush on file what's flushable */
	FlushFile(mb,shift,shift_zero);
    
	if (saturation(mb))
    {
		warning_message(WARNING_SATURATION,
						"Saturation on %s-%s\n",
						name_Phone(LeftPhone(prev_diph(mb))),	
						name_Phone(RightPhone(prev_diph(mb))));
		saturation(mb)=False;
    }
  
	/* !! SHIFTING CAN BE REMOVED IN CASE OF STATIC OUTPUT BUFFER !! */
	/* shift the ola window and completion with 0 */
	memmove(&ola_win(mb)[0], &ola_win(mb)[shift], end_window*sizeof(ola_win(mb)[0]));
	for(k=end_window; k<2*MBRPeriod(diph_dba(mb)) ; k++) 
		ola_win(mb)[k]=0.0f;

	/* Two kinds of OLA depending if the frame is unvoiced */
	type= pmrk_DiphoneSynthesis(prev_diph(mb), frame_number(mb)[frame] );
  
	debug_message5("Frame type=%i Number=%i Real=%i Smooth=%i\n",
				   type,
				   frame_number(mb)[frame],
				   real_frame(prev_diph(mb))[frame_number(mb)[frame]],
				   smooth(prev_diph(mb)));

	if (! (type & VOICING_MASK))
    {	/* UNVOICED FRAME => PLAY BACK */
		if ( (frame<=nb_begin(mb))||
			 (frame>lim_smooth))
			warning_message(ERROR_SMOOTHING,"Smooth Panic! Stay cool");
		
		/* Check if we're duplicating an unvoiced frame */
		if ( odd(mb) && 
			 (frame_number(mb)[frame] == frame_number(mb)[frame-1]))
		{
			/* reverse every second duplicated UV frame */
			add_window=add_window+2*MBRPeriod(diph_dba(mb))-1;
			for (k=0; k< 2*MBRPeriod(diph_dba(mb)) ; k++)
			{
				tmp = weight(mb)[k] * (float) buffer(prev_diph(mb))[add_window - k];
				  
				/* Energy correction  */
				tmp *= correction;
				ola_win(mb)[k] += tmp;
			}
		}
		else		  /* Don't reverse the unvoiced frame */
		{
			for (k=0; k< 2*MBRPeriod(diph_dba(mb)) ; k++)
			{
				tmp = weight(mb)[k] * (float) buffer(prev_diph(mb))[add_window + k];
				  
				/* Energy correction  */
				tmp *= correction;
				ola_win(mb)[k] += tmp;
			}
		}
    }
	else
    { 
		/* Meeep meeep, you're entering a restrictead area, helmet is
		 * recommended if you don't have rights to exploit Belgian
		 * patent BE09600524 or US Patent Nr. 5,987,413
		 *
		 * Voiced frame -> autoloop ! MBROLA unique feature !!
		 * Many case depending on smoothing or not
		 */       
	   
		if ((frame<=nb_begin(mb)) && 
			smooth(prev_diph(mb)) && 
			smoothing(mb) )
			/* Left smoothing */
		{
			float smooth_left = (float)(nb_begin(mb)-frame+1) / (2*(float)nb_begin(mb));
	  
			for (k=0; k<MBRPeriod(diph_dba(mb)) ; k++)
				ola_win(mb)[k] += correction * 
					( weight(mb)[k] * (float)buffer(prev_diph(mb))[add_window + k]
					  + smooth_left * smoothw(prev_diph(mb))[k] );
	  
			for ( ; k<2*MBRPeriod(diph_dba(mb)) ; k++)
				ola_win(mb)[k] += correction * 
					( weight(mb)[k] * (float) buffer(prev_diph(mb)) [add_window + k - MBRPeriod(diph_dba(mb))]
					  + smooth_left * smoothw(prev_diph(mb))[k] );
		}
		else if ( (frame>lim_smooth)   && 
				  smooth(cur_diph(mb)) &&
				  smoothing(mb) )
			/* Right smoothing */
		{
			float smooth_right= (float)(nb_end(mb)-(nb_pm(prev_diph(mb))-frame))
				/(2*(float)nb_end(mb));

			for (k=0; k<MBRPeriod(diph_dba(mb)) ; k++)
				ola_win(mb)[k] += correction * 
					( weight(mb)[k] * (float)buffer(prev_diph(mb))[add_window + k]
					  - smooth_right * smoothw(cur_diph(mb)) [k]);
			 
			for ( ; k<2*MBRPeriod(diph_dba(mb)) ; k++)
				ola_win(mb)[k] += correction * 
					( weight(mb)[k] * (float) buffer(prev_diph(mb)) [add_window + k - MBRPeriod(diph_dba(mb))]
					  - smooth_right * smoothw(cur_diph(mb))[k]);
		}
		else 
			/* No smoothing */
		{
			for (k=0; k<MBRPeriod(diph_dba(mb)) ; k++)
				ola_win(mb)[k] += correction * weight(mb)[k] * 
					(float)buffer(prev_diph(mb))[add_window + k];
			for ( ; k<2*MBRPeriod(diph_dba(mb)) ; k++)
				ola_win(mb)[k] += correction * weight(mb)[k] * 
					(float) buffer(prev_diph(mb))[add_window + k - MBRPeriod(diph_dba(mb))];
		}
    }  
  
	odd(mb)= !odd(mb);							  /* Flip flop for NV frames */

	debug_message1("done OverLapAdd\n");
}


#ifdef LIBRARY

/* LIBRARY mode: synthesis driven by the output */

int readtype_Mbrola(Mbrola* mb, void *buffer_out, int nb_wanted, AudioType sample_type)
/*
 * Reads nb_wanted samples in an audio buffer
 * Returns the effective number of samples read
 *
 * negative means error code
 */
{
	int to_go= nb_wanted; /* number of samples to go */
	int nb_move;
    
	if (first_call(mb))
    {
		odd(mb)=0;
		eaten(mb)=0;
		buffer_shift(mb)=0;
		zero_padding(mb)=0;
		frame_counter(mb)=1;
		if (!reset_Mbrola(mb))
			return lasterr_code;
		
		/* Test if there is something in the buffer */
		switch (  NextDiphone(mb) )
		{
		case PHO_OK: /* go ahead */
			break;
		case PHO_ERROR:  /* return error code */
			return lasterr_code;
		case PHO_EOF:
		case PHO_FLUSH:  /* Flush or EOF, 0 samples */
			return 0;      
		default:
			/* panic ! */
			fatal_message(ERROR_NEXTDIPHONE,"NextDiphone PANIC %i!\n");
			return ERROR_NEXTDIPHONE ; 
		}
		first_call(mb)=False;
    }
  
	while (to_go>0)
    {
		/* Zero padding between 2 Ola frames for extra low pitch */
		if (zero_padding(mb)>0)
		{
			int nb_generated;
			 
			if ( zero_padding(mb) > to_go)
				nb_generated= to_go;
			else
				nb_generated= zero_padding(mb);

			/* decrement in case the requested buffer is very small */
			zero_padding(mb)-= nb_generated; 
			 
			buffer_out= zero_convert( buffer_out,nb_generated, sample_type );
			if (!buffer_out)
				return lasterr_code;
			to_go -= nb_generated;
		}
		
		/* Available frames from previous OLA */
		if (to_go > (buffer_shift(mb)-eaten(mb)))
			nb_move= buffer_shift(mb)-eaten(mb);
		else
			nb_move= to_go;
		
		buffer_out= move_convert( buffer_out, &ola_integer(mb)[eaten(mb)], nb_move, sample_type );
		if (!buffer_out)
			return lasterr_code;

		to_go-= nb_move;
		eaten(mb)+= nb_move;
		
		if (to_go<=0)
			break;
		
		/* Still some frames available in the same diphone ? */
		if (frame_counter(mb)<nb_pm(prev_diph(mb)))
			frame_counter(mb)++;
		else
		{ /* Else, nextdiphone */
			StatePhone stream_state;
			 
			stream_state= NextDiphone(mb);
			 
			if (stream_state != PHO_OK)
			{
				/* handle errors in the parser */
				if (stream_state == PHO_ERROR)
					return lasterr_code;
	      
				/* Flush or EOF */
				if (stream_state == PHO_FLUSH)
					first_call(mb)=True;
				break;
			}
			 
			if ( !MatchProsody(mb) )
				return lasterr_code;
			 
			Concat(mb);
			odd(mb)=0;
			frame_counter(mb)=1;
		}
		
		eaten(mb)=0;

		/* condition against phonemes with 0 length */
		if (frame_counter(mb)<=nb_pm(prev_diph(mb)))
			OverLapAdd(mb, frame_counter(mb));
    }
	/* old C++ catch throw  "}  catch(int ret) { return ret;  }"  */
	return(nb_wanted - to_go);
}

#else

/* 
 * STANDALONE MODE: Synthesis driven by the input 
 *
 * WARNING: we don't care of error value returned, as fatal_messages
 *          are really fatal in standalone, call exit()
 */

void oneshot_Mbrola(Mbrola* mb)
/*
 * Completely perform MBROLA synthesis of prev_diph(mb)
 * Different from audio chunks synthesized on demand in library mode 
 */
{
	debug_message1("Mbrola\n");

	MatchProsody(mb); /* we're in standalone */
	debug_message3("%s-%s Concat : ", 
				   left_diphone(mb, prev_diph(mb)), 
				   right_diphone(mb, prev_diph(mb)));
	Concat(mb);

#ifdef DEBUG
	{
		int frame_counter;
    
		debug_message4("%s-%s NbPM :%u ", 
					   left_diphone(mb,prev_diph(mb)), 
					   right_diphone(mb,prev_diph(mb)),
					   nb_pm(prev_diph(mb)));
    
		for (frame_counter=1; frame_counter<=nb_pm(prev_diph(mb)); frame_counter++)
			debug_message4("(%u %u %u) ",	
						   frame_number(mb)[frame_counter], 
						   pmrk_DiphoneSynthesis(prev_diph(mb), 
												 real_frame(prev_diph(mb))[frame_number(mb)[frame_counter]]), 
						   frame_pos(mb)[frame_counter]-frame_pos(mb)[frame_counter-1]);
		debug_message1("\nSamples : ");
	}
  
#endif
  
	odd(mb)=False;
	for (frame_counter(mb)=1; frame_counter(mb)<=nb_pm(prev_diph(mb)); frame_counter(mb)++)
    {
#ifdef SIGNAL
		if (must_flush) /* test if premature return requested (reset signal) */
			return;
#endif
		OverLapAdd(mb,frame_counter(mb));
    }
  
#ifdef DEBUG
	debug_message1("\n");
	debug_message4("DEBUG: LeftPhone=%s RightPhone=%s genere %i\n", 
				   left_diphone(mb , prev_diph(mb)),
				   right_diphone(mb , prev_diph(mb)),
				   frame_counter(mb));
	debug_message3("DEBUG: LeftPhone=%i RightPhone=%i\n", 
				   Length1(prev_diph(mb)),
				   Length2(prev_diph(mb)));
	debug_message3("DEBUG: Patternpointsleft=%i Right=%i \n", 
				   NPitchPatternPoints(LeftPhone(prev_diph(mb))),
				   NPitchPatternPoints(RightPhone(prev_diph(mb))));
  
	if (NPitchPatternPoints(LeftPhone(prev_diph(mb))) != 0)
		debug_message3("DEBUG: firstpitch=%f lastpitch=%f \n", 
					   freq_Pitch(head_PitchPattern((LeftPhone(prev_diph(mb))))),
					   freq_Pitch(tail_PitchPattern((LeftPhone(prev_diph(mb))))));
	debug_message1("done Mbrola\n");
#endif
}

StatePhone Synthesis(Mbrola* mb)
/*
 * Main loop: performs MBROLA synthesis of all diphones
 * Returns a value indicating the reasons of the break
 * (a flush request, a end of file, end of phone sequence)
 */
{
	StatePhone stream_state;	/* Indicate a # or eof has been encountered in the   */
	/* command file -> flush and continue         */

	debug_message1("Synthesis\n");
  
	/* 
	 * Put something in the pipe, and initialization of junk diphone cur_diph
	 * that will be prev_diph at time of Concat -> needed for "buff_left"
	 */
	NextDiphone(mb);  /* standalone mode */

#ifndef SIGNAL
	while ((stream_state=NextDiphone(mb)) == PHO_OK)
		oneshot_Mbrola(mb);
#else
	while ( ((stream_state=NextDiphone(mb)) == PHO_OK) &&
			!must_flush )
		oneshot_Mbrola(mb);
  
	/* Reset signal */
	if (must_flush)
    {
		/* Eat remaining phones before a flush */
		while (stream_state == PHO_OK)
		{
			stream_state= NextDiphone(mb);
		}
      
		warning_message(1,"Input Flush Signal\n");
		must_flush=False; /* reset until next call ! */
    }
#endif
  
	debug_message1("done Synthesis\n");
	return(stream_state);
}

#endif
