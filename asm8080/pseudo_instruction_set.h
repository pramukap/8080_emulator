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

#define NO_PSEUDO_FOUND (-1)
#define	ORG		  0
#define	EQU		  1
#define END		  2

#define NUM_PSEUDO	  3	//# of available pseudo-instructions

/*
typdef enum pseudo_instruction
{
	ORG,
	EQU,
	SET,
	END,
	DB,
	DW,
	DS,
	
}
*/

typedef struct pseudo_instruction
{
	char *mnemonic;
	int identifier;		
}
pseudo;

pseudo pseudo_instruction_set[NUM_PSEUDO] = 
{
	{"org ", ORG},
	{"equ ", EQU},
	{"end ", END}
};

//finds pseudo-instruction in the line and replaces it with whitespace
//returns an value identifying the pseudo-instruction
int FindPseudoInstruction(char *line)
{
	int 	i,j,
		pseudo_length;
	pseudo 	p;

	for(i = 0; i< NUM_PSEUDO; i++)
	{
		p = pseudo_instruction_set[i];
		pseudo_length = strlen(p.mnemonic);
	
		if(strncmp(p.mnemonic, line, pseudo_length) == MATCH)
		{
			for(j = 0; j < pseudo_length; j++)
			{
				line[j] = ' ';
			}
			return p.identifier;
		}
	}

	return NO_PSEUDO_FOUND;
}
