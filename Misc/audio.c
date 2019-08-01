/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    audio_file.c
 * Purpose: audio files
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
 * 29/02/96 : Created. Put here input output function depending on the
 *            little endian or big endian format
 *            Also audio_file header writing and file format extraction
 * 09/04/98 : split to little_big
 * 
 * 17/06/98 : linear16to8 corrected (gave alaw result )
 *
 */

#include "common.h"
#include "audio.h"
#include "little_big.h"
#include "g711.h"

/* True if the format of the current output needs byte swapping */
bool audio_swapped;

#ifdef LIBRARY

int16* conv_LIN16fromLIN16( int16 *out, int16 *in, int nb_move)
/* linear 16bits to linear16 :-) simply move */
{
	memmove((int16 *) out , (int16 *) in , nb_move*2);
	return( &out[nb_move] );
}

int8 linear2lin8(int16 lin)
/* linear16tolinear8 */
{
	return( 128 + (lin >> 8));
}

int8* conv_LIN8fromLIN16( int8 *out, int16 *in, int nb_move)
/* linear 16bits to linear8 */
{
	int i;
	for(i=0; i<nb_move; i++)
		out[i]= linear2lin8(in[i]);
	return( &out[i]);
}

int8* conv_ULAWfromLIN16( int8 *out,	int16 *in, int nb_move)
/* linear 16bits to mulaw */
{
	int i;
	for(i=0; i<nb_move; i++)
		out[i]= linear2ulaw(in[i]);
	return( &out[i] );
}

/* linear 16bits to alaw */
int8* conv_ALAWfromLIN16( int8 *out, int16 *in, int nb_move)
{
	int i;
	for(i=0; i<nb_move; i++)
		out[i]= linear2alaw(in[i]);
	return( &out[i] );
}

void* zero_convert(void* buffer_out, int nb_move, AudioType sample_type)
/*
 * Output zeros in a buffer according to the sample_type
 * Return the next position after the end of the buffer
 *
 * Returning NULL means fatal error
 */
{
	int i;
	int8 value8bits;
  
	switch(sample_type)
    {
		/* Get rid of the 16 bits case */
    case LIN16 :
		for(i=0 ; i<nb_move; i++) 
			((int16*)buffer_out)[i]=0;
		
		return ( &(((int16*)buffer_out)[i]) );
		
		/* Convert the type */
    case LIN8 :
		value8bits= linear2lin8(0);
		break;
		
    case ULAW :
		value8bits= linear2ulaw(0);
		break;
		
    case ALAW :
		value8bits= linear2alaw(0);
		break;
		
    default:
		fatal_message(WARNING_UPGRADE,"Unknown sample type");
		return NULL;
    }  
  
	/* treat all the cases on 8 bits */
	for(i=0 ; i<nb_move; i++) 
		((int8*)buffer_out)[i]= value8bits;
  
	return( (void*) &(((int8*)buffer_out)[i]));
}


void* move_convert(void* buffer_out,int16* buffer_in,int nb_move, AudioType sample_type)
/* 
 * Move audio samples and convert them at the same time 
 * Return the shifted pointer in buffer_out
 *
 * linear 16bits to linear16 :-) simply move 
 * linear 16bits to linear8 
 * linear 16bits to mulaw 
 * linear 16bits to alaw 
 *
 * Returning NULL means fatal error
 */
{
	switch(sample_type)
    {
		/* simply move the bytes */
    case LIN16 :
		return (void*) conv_LIN16fromLIN16( (int16 *)buffer_out,
											buffer_in,
											nb_move) ;

		/* Convert the type */
    case LIN8 :
		return( (void*) conv_LIN8fromLIN16( (int8 *)buffer_out,
											buffer_in,
											nb_move));
    case ULAW :
		return( (void*) conv_ULAWfromLIN16( (int8 *)buffer_out,
											buffer_in,
											nb_move));
    case ALAW :
		return( (void*) conv_ALAWfromLIN16( (int8 *)buffer_out,
											buffer_in,
											nb_move));
    default:
		fatal_message(WARNING_UPGRADE,"Unknown sample type");
		return NULL; /* to please the compiler */
    }
}

#endif

int write_int16s(int16 *buffer,int count,FILE *file)
/* Write a buffer of int16 */
{
	if (audio_swapped)
		swab( (char *) buffer, (char *) buffer, count*2);
  
	return(fwrite(buffer, sizeof(int16), count, file));
}

void write_header(WaveType file_format, int32 audio_length, uint16 samp_rate, FILE *output_file)
/* Write the header corresponding to the output audio format */
{
	switch(file_format)
    {
		/* 
		 * RAW= Write nothing ! 
		 */
    case RAW_FORMAT: 
		audio_swapped=False; /* Raw is the format of the machine= no swap */
		return;	  
		
		/* 
		 * Write a WAV file for PCs 
		 */
    case WAV_FORMAT: 
#ifdef BIG_ENDIAN
		audio_swapped=True;
#else
		audio_swapped=False;
#endif
		fputs("RIFF", output_file);
		writel_int32(audio_length*sizeof(int16)+44-8, output_file);	/* total len */
		fputs("WAVE", output_file);
		fputs("fmt ", output_file);
		writel_int32(16, output_file);		  /* fmt chunk size */
		writel_int16(1, output_file);	        /* FormatTag: WAVE_FORMAT_PCM */
		writel_int16(1, output_file);         /*  channels"\001\000"        */
		writel_int32(samp_rate, output_file); /* SamplesPerSec             */
		writel_int32(samp_rate*2, output_file); /*  Average Bytes/sec        */
		writel_int16(2, output_file);         /*  BlockAlign "\002\000"    */
		writel_int16(16, output_file);	     /* BitsPerSample  "\020\000" */ 
		fputs("data", output_file);
		writel_int32(audio_length*sizeof(int16),output_file);  /* data chunk size */
		return;

		/* 
		 * Write an AIF file for SGIs
		 */
    case AIF_FORMAT:
    case AIFF_FORMAT:
#ifdef BIG_ENDIAN
		audio_swapped=False;
#else
		audio_swapped=True;
#endif
		fputs ("FORM",output_file);  /* MAGIC of AIF files */
		writeb_int32 (audio_length*sizeof(int16)+54-8, output_file); /* Form size */
		fputs ("AIFF",output_file);  /* Form type */

		fputs ("COMM",output_file);  /* Common chunk */
		writeb_int32 (18, output_file); /* Chunk size */
		writeb_int16(0x1,output_file);	  /* Number of channels */
		writeb_int32 (audio_length, output_file);	/* Num of samples */
		writeb_int16 (16, output_file); /* Sample size */

		/* Write FS=16000 Hertz  */
		writeb_int32 (0x400cfa00 , output_file);	/* Sample rate= extended 80bits */
		writeb_int32 (0x00000000 , output_file); /* Sample rate= extended 80bits */
		writeb_int16 (0x0000 , output_file);  /* Sample rate= extended 80bits */

		fputs ("SSND",output_file);  /* Sound chunk */
		writeb_int32(audio_length*sizeof(int16)+8, output_file); /* Chunk size */
		writeb_int32(0x0 , output_file); /* Offset */
		writeb_int32(0x0 , output_file); /* Block size */
		return;

		/* 
		 * Write a AU file for SUNs and NEXTs
		 */
    case AU_FORMAT:
#ifdef BIG_ENDIAN
		audio_swapped=False;
#else
		audio_swapped=True;
#endif
		fputs(".snd", output_file);                  /* MAGIC of AU files */
		writeb_int32( 7*sizeof(int32), output_file); /* Header size */
		if (audio_length==0)		 
			writeb_int32( ((unsigned)(~0)), output_file); /* AUDIO_UNKNOWN_SIZE */
		else
			writeb_int32(audio_length*sizeof(int16), output_file); 
		writeb_int32( 3 , output_file);     /*  16-bit linear PCM */
		writeb_int32( samp_rate, output_file);	/* sample rate */
		writeb_int32( 1 , output_file);	   /* channels */
		fputs("MBRP", output_file);         /* optional text information */
		return;

    default: 
		return;
    }
}

void LowerCase(char *string,char *lower_case)
/* Lowercase of the string */
{
	int i=0;
  
	do
    {
		if ('A'<=string[i] && string[i]<='Z')
			lower_case[i]= (char) (string[i] + 'a' - 'A');
		else 
			lower_case[i]=string[i];
    }
	while (string[i++]!=(char)0x0);
}

WaveType find_file_format(char *name)
/* 
 * Find the file format corresponding to the name's extension 
 *	raw=none wav=RIFF au=Sun Audio aif or aiff=Macintosh
 */
{ 
	char *lower_case;
	static char *format_list[] = { ".raw", ".wav", ".au", ".aif", ".aiff",  NULL };
	int i, l,ll;
  
	l=strlen(name);
	lower_case= (char *) MBR_malloc(1+l*sizeof(char));
	LowerCase(name,lower_case);

	/* Find the right extension */
	for (i=0; format_list[i]!=NULL ; i++)
    {
		ll=strlen(format_list[i]);
		if ((l>ll) && !strcmp(lower_case+l-ll, format_list[i]))
			break;
    }
	MBR_free(lower_case);
  
	if (format_list[i] == NULL)	  /* no format means RAW */
		return(RAW_FORMAT);
  
	return((WaveType)i);
}
