/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    diphone.c
 * Time-stamp: <00/03/30 01:29:26 pagel>
 * Purpose: Phone and diphone objects
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
 *
 * 15/06/98 : Created. Phones, Diphones, ...
 * 09/09/98 : reset_DiphoneSynthesis function
 * 24/03/00 : no more limits on period size -> 
 *                  pass frame size during construction
 */

#include "diphone.h"

DiphoneSynthesis* init_DiphoneSynthesis(int mbr_period, int max_pm, int max_samples)
/*  Alloc memory, working and audio buffers for synthesis */
{
	DiphoneSynthesis* self;
  
	self= (DiphoneSynthesis *) MBR_malloc(sizeof(DiphoneSynthesis));
  
	LeftPhone(self)=NULL;			  
	RightPhone(self)=NULL;
  
	smoothw(self)= (int16*) MBR_malloc(sizeof(int16) * 2*mbr_period);
	real_frame(self)= (uint8*) MBR_malloc(sizeof(uint8) * max_pm);
  
	/* For ROM RAW databases, no need to alloc a buffer  */
	if (max_samples)
		buffer(self)= (int16*) MBR_malloc(sizeof(int16) * max_samples);
	buffer_alloced(self)= (max_samples!=0);
  
	return(self);
}

void reset_DiphoneSynthesis(DiphoneSynthesis* ds)
/*
 * Forget the diphone in progress
 */
{
	ds->Descriptor=NULL;
  
	if (LeftPhone(ds))
    {
		close_Phone( LeftPhone(ds) );
		LeftPhone(ds)=NULL;
    }
  
	if (RightPhone(ds))
    {
		close_Phone( RightPhone(ds) );
		RightPhone(ds)= NULL;
    }
}

void close_DiphoneSynthesis(DiphoneSynthesis* ds)
/* Release memory and phone */
{
	reset_DiphoneSynthesis(ds);
  
	if (smoothw(ds))
		MBR_free( smoothw(ds) );
  
	if  ( buffer_alloced(ds) &&
		  buffer(ds) )
		MBR_free( buffer(ds) );
  
	if (real_frame(ds))
		MBR_free( real_frame(ds) );
  
	MBR_free(ds);
}

int GetPitchPeriod(DiphoneSynthesis *dp, int cur_sample,int Freq)
/*
 * Returns the pitch period (in samples) at position cur_sample 
 * of dp by linear interpolation between pitch pattern points.
 */
{
	int i;
	Phone *work_phone;
	float phon_time;		/* Time from the begining of the phoneme */
	float curtime= cur_sample*1000.0f/(float)Freq; /* Time in ms */
	float return_val;
  
	debug_message2("GetPitch %f\n",
				   curtime);
  
	/* Left part of the diphone */
	if (cur_sample < Length1(dp))
    {
		work_phone=LeftPhone(dp);
		phon_time= curtime + length_Phone(LeftPhone(dp)) - 
			Length1(dp)/(Freq/1000); 
      
		/* Too far */
		if (phon_time >= length_Phone(LeftPhone(dp)))
		{
			Phone *phon=LeftPhone(dp);
			return (int) ((float)Freq/freq_Pitch(tail_PitchPattern(phon)));
		}
    }
	else		/* Right part of the diphone */
    {
		work_phone=RightPhone(dp);
		phon_time= curtime - Length1(dp)/(Freq/1000) ;
    }
  
	if (phon_time<0.0f) 
		phon_time=0.0f;
  
	i=0;
	while ( (i < work_phone->NPitchPatternPoints) &&
			(phon_time >= work_phone->PitchPattern[i].pos))
		i++;
  
	if (i>=work_phone->NPitchPatternPoints)
		return_val= freq_Pitch(tail_PitchPattern(work_phone));
	else	/* Linear interpolation of pitch value */
		return_val= work_phone->PitchPattern[i-1].freq
			+ (work_phone->PitchPattern[i].freq - work_phone->PitchPattern[i-1].freq)
			/(work_phone->PitchPattern[i].pos - work_phone->PitchPattern[i-1].pos)
			* (phon_time - work_phone->PitchPattern[i-1].pos);

	debug_message1("done GetPitch\n");
	return( (int)((float)Freq / return_val) );
}
