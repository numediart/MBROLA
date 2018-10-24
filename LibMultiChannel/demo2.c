/*
 * FPMs-TCTS SOFTWARE LIBRARY
 * Time-stamp: <1998-10-21 16:19:14 pagel>
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
 * File:  demo2.c
 * Purpose: demo of multiple channel synthesis
 * Authors: Pagel Vincent
 * Email : mbrola@tcts.fpms.ac.be
 *
 * 01/09/98: Created
 */

#include <stdio.h>
#include <stdarg.h>

#include "common.h"
#include "parser.h"

/* Those includes depend on the parser you are using */
#include "input_fifo.h"
#include "parser_input.h"
#include "input_fifo.h"
/* end of default parser include */

#include "multichannel.h"
#include "mbrola.h"

short buffer[16000];

#define True 1
#define False 0

void handle_error(int Fatal)
{
	char err[255];
  
	lastErrorStr_MBR2(err,sizeof(err));
  
	printf("Code %i\n%s\n", lastError_MBR2(), err);

	if (Fatal)
		exit(-1);
}

int WriteSpeechFile(Mbrola* mb,FILE *output, AudioType type, char size)
/* Read audio while available (out of input data or flush */
{
	int i;
	
	while ((i=readtype_MBR2(mb,buffer,16000,type))==16000)
		fwrite(buffer,size,i,output);		
	
	if (i>0)
	{
		fwrite(buffer,size,i,output);	
		return 0;
	}
	else
	{
		return i;
	}
}

int main(int argc, char **argv)
{
	/* Audio output file for each channel */
	FILE *output1;
	FILE *output2;
	FILE *output3;

	/* The diphone database */
	Database* main_dba;      /* the main instance */
	Database* clone_dba1;    /* secondary instances for different channels */
	Database* clone_dba2;
	Database* clone_dba3;
  
	/* Parser providing the phonemic input to the engine */
	Parser* parser1;
	Parser* parser2;
	Parser* parser3;

	/* Fifo associated with each Input */
	Fifo* fifo1;
	Fifo* fifo2;
	Fifo* fifo3;

	/* Input associated with each parser */
	Input* input1;
	Input* input2;
	Input* input3;

	/* One synthesis engine for each channel */
	Mbrola* channel1;
	Mbrola* channel2;
	Mbrola* channel3;
  
	if (argc!=2)
	{
		printf("synthlib ../fr1\n");
		return 1;
	}
  
	/* open the dba with no renaming */
	main_dba= init_DatabaseMBR2(argv[1],NULL,NULL); 
	if (!main_dba)
		handle_error(True);

	output1=fopen("res1.ulaw","wb");
	output2=fopen("res2.raw","wb");
	output3=fopen("res3.lin8","wb");

	/* 
	 * The section of code below depends on your implementation 
	 * of the phoneme parser. We use here the Mbrola default
	 * parser, but you could use your own (* as long as it
	 * follows the generic parser.h interface *)
	 */

	/* Input Fifo with a buffer of 100 chars */
	fifo1= init_Fifo(100);
	fifo2= init_Fifo(100);
	fifo3= init_Fifo(100);

	/* Input stream of the synthesizer */
	input1= init_InputFifo(fifo1);
	input2= init_InputFifo(fifo2);
	input3= init_InputFifo(fifo3);
  
	/* Plug the fifos on the default parsers */
	parser1= init_ParserInput(input1,"_",120.0,1.0,1.0,";",NULL);
	parser2= init_ParserInput(input2,"_",120.0,1.0,1.0,";",NULL);
	parser3= init_ParserInput(input3,"_",120.0,1.0,1.0,";",NULL);
  
	/* 
	 * End of parser dependent part
	 */

	/*
	 * Build the databases for each channel, the database handler 
	 * occupies little memory
	 */
	clone_dba1= copyconstructor_DatabaseMBR2(main_dba);
	clone_dba2= copyconstructor_DatabaseMBR2(main_dba);
	clone_dba3= copyconstructor_DatabaseMBR2(main_dba);

	if (!clone_dba1 ||
		!clone_dba2 ||
		!clone_dba3)
		handle_error(True);
  
	/* 
	 * Plug everything in the different engines, they all use 
	 * the same physical database 
	 */
	channel1= init_MBR2(clone_dba1,parser1);
	channel2= init_MBR2(clone_dba2,parser2);
	channel3= init_MBR2(clone_dba3,parser3);

	if (!channel1 ||
		!channel2 ||
		!channel3)
		handle_error(True);
 
	/* 
	 * Send the data to synthesize
	 */

	/* write_Fifo(fifo1,";; F=0.2\n"); */
	if ((write_Fifo(fifo1,"_ 51 \n b 62  \n")<0)          ||
		(write_Fifo(fifo1,"o~ 127  50 170 \n Z 110\n")<0) ||
		(WriteSpeechFile(channel1,output1,ULAW,1)<0)      ||
		(write_Fifo(fifo1,"u 211 100 200\n R 150 \n_ 9\n#\n")<0)||
		(WriteSpeechFile(channel1,output1,ULAW,1)))
		handle_error(False);

	if ((write_Fifo(fifo2,"_ 51 \n b 62  \n")<0)                 ||
		(write_Fifo(fifo2,"o~ 127  50 170 \n Z 110\n")<0)        ||
		(WriteSpeechFile(channel2,output2,LIN16,2)<0)            ||
		(write_Fifo(fifo2,"u 211 100 200\n R 150 \n_ 9\n#\n")<0) ||
		(WriteSpeechFile(channel2,output2,LIN16,2)<0))
		handle_error(False);

	if ((write_Fifo(fifo3,"_ 51 \n b 62  \n")<0) ||
		(write_Fifo(fifo3,"o~ 127  50 170 \n Z 110\n")<0) ||
		(WriteSpeechFile(channel3,output3,LIN8,1)<0) ||
		(write_Fifo(fifo3,"u 211 100 200\n R 150 \n_ 9\n#\n")<0) ||
		(WriteSpeechFile(channel3,output3,LIN8,1)<0))
		handle_error(False);

	/*
	 * It's over, release the memory
	 */
	close_MBR2(channel1);
	fclose(output1);
	close_MBR2(channel2);
	fclose(output2);
	close_MBR2(channel3);
	fclose(output3);

	/* Close the database */
	close_DatabaseMBR2(clone_dba1);
	close_DatabaseMBR2(clone_dba2);
	close_DatabaseMBR2(clone_dba3);

	/* The main instance MUST'nt be closed before any of its clone */
	close_DatabaseMBR2(main_dba);

	/* Close polymorhpic parser */
	close_ParserMBR2(parser1);
	close_ParserMBR2(parser2);
	close_ParserMBR2(parser3);
  
	/*
	 * The code below is dependent on the parser implementation
	 */
  
	input1->close_Input(input1);
	input2->close_Input(input2);
	input3->close_Input(input3);
  
	close_Fifo(fifo1);
	close_Fifo(fifo2);
	close_Fifo(fifo3);
	/*
	 * End of parser dependent code
	 */
  
	return(0);
}
