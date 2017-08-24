/********************************************************************************
 * 8080 Pseudo-Instruction Set Object for the 8080 Assembler			*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * 8080's assembler level instructions:						*
 * Pseudo-instructions help the assembler put the program into the computer	*
 ********************************************************************************/

#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

//#define NO_PSEUDO_FOUND (-1)
//#define	ORG		  0
//#define	EQU		  1
//#define END		  2

#define NUM_PSEUDO	  5	//# of available pseudo-instructions

typedef struct pseudo_instruction_data
{
	char *mnemonic;
	pseudo_identifier identifier;		
}
pseudo_data;

pseudo_data pseudo_instruction_set[NUM_PSEUDO] = 
{
	{"org", ORG},
	{"equ", EQU},
	{"db" ,  DB},
	{"dw" ,  DW},	
	{"end", END}
};

//finds pseudo-instruction in the line and replaces it with whitespace
//returns an value identifying the pseudo-instruction
pseudo_identifier FindPseudoInstruction(char *line)
{
	int 	i,j,k,
		line_length,
		pseudo_length;
	pseudo_data 	p;

	line_length = strlen(line);

	//iterate through pseudo_instruction_set
	for(i = 0; i< NUM_PSEUDO; i++)
	{
		p = pseudo_instruction_set[i];
		pseudo_length = strlen(p.mnemonic);

		//iterate through line
		for(j = 0; pseudo_length + j <= line_length; j++)	
		{
			if(strncmp(p.mnemonic, line + j, pseudo_length) == MATCH)
			{
				//check that matching part of line is not within another word (ex. "ORG" in "ORGAN")
				if(strlen(line + j) > pseudo_length)	
				{
					if(line[j + pseudo_length] == ' ' 
					|| line[j + pseudo_length] == TAB 
					|| line[j + pseudo_length] == '\a') 
					{
						for(k = j; k < j + pseudo_length; k++)
						{
							line[k] = ' ';
						}
						return p.identifier;
					}
				}
				else
				{
					return p.identifier;
				}
			}
		}
	}

	return NO_PSEUDO_FOUND;
}
