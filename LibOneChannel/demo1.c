/*
 * Time-stamp: <1999-06-11 16:35:14 pagel>
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
 * File:  demo1.c
 * Purpose: demo of multiple channel synthesis
 * Authors: Pagel Vincent
 * Email : mbrola@tcts.fpms.ac.be
 *
 * 01/09/98: Created again
 */

#include <stdio.h>
#include <stdarg.h>


#include "common.h"
#include "parser_export.h"
#include "onechannel.h"
#include "incdll.h"

short buffer[16000];

void handle_error()
{
	char err[255];
  
	lastErrorStr_MBR(err,sizeof(err));
  
	printf("Code %i\n%s\n", lastError_MBR(), err);
	exit(-1);
}

int WriteSpeechFile(FILE *output, AudioType type, char size)
{
	int i;

	while ((i=readtype_MBR(buffer,16000,type))==16000)
		fwrite(buffer,size,i,output);		

	if (i>0)
	{
		fwrite(buffer,size,i,output);	
		return 0;
	}
	else
		return i;
}

int main(int argc, char **argv)
{
	FILE *output;
  
	if (argc!=2)
	{
		printf("%s ../fr1\n",argv[0]);
		return 1;
	}
  
	if ( init_MBR(argv[1])<0 )
		handle_error();
  
	output=fopen("res.ulaw","wb");
	/* write_MBR(";; F=0.2\n"); */

	if (  (write_MBR("_ 51 \n b 62  \n") < 0)         ||
		  (write_MBR("o~ 127  50 170 \n Z 110\n") <0) ||
		  (WriteSpeechFile(output,LIN16,2)<0)         ||
		  (write_MBR("u 211 100 200\n R 150 \n_ 9\n#\n") < 0) ||
		  (WriteSpeechFile(output,LIN16,2)<0))
		handle_error();

	close_MBR();
	fclose(output);

	return(0);
}

/* 51+ 62+ 127+ 110+ 211+ 150+ 9= 720, should be 720*16*2=23040 samples */
