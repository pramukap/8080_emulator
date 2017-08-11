/********************************************************************************
 * 8080 Instruction Set Object for the 8080 Assembler				*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * 8080's instruction set:							*
 * Used by the assembler to find the opcode for a given mnemonic	 	*
 ********************************************************************************/

#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

#define ARRAY_SIZE 256
#define	BEFORE	(-1)
#define AFTER	1

/*
#define	NONE	0
#define	R	1
#define	RP	2
#define	D8	3
#define	D16	4
#define	ADDR	5
*/

typedef struct instruction_information
{
	char* mnemonic;
	uint8_t opcode;
	uint8_t operand_bytes;
	operand_type type;
}
instruction;


instruction instruction_set[ARRAY_SIZE] =
{
	/*00*/	{"aci", 0xce, 1, D8},
		{"adc a", 0x8f, 0, NONE},
	/*02*/	{"adc b", 0x88, 0, NONE},
		{"adc c", 0x89, 0, NONE},
	/*04*/	{"adc d", 0x8a, 0, NONE},
		{"adc e", 0x8b, 0, NONE},
	/*06*/	{"adc h", 0x8c, 0, NONE},
		{"adc l", 0x8d, 0, NONE},
	/*08*/	{"adc m", 0x8e, 0, NONE},
		{"add a", 0x87, 0, NONE},
	/*0a*/	{"add b", 0x80, 0, NONE},
		{"add c", 0x81, 0, NONE},
	/*0c*/	{"add d", 0x82, 0, NONE},
		{"add e", 0x83, 0, NONE},
	/*0e*/	{"add h", 0x84, 0, NONE},
		{"add l", 0x85, 0, NONE},
	/*10*/	{"add m", 0x86, 0, NONE},
		{"adi", 0xc6, 1, D8},
	/*12*/	{"ana a", 0xa7, 0, NONE},
		{"ana b", 0xa0, 0, NONE},
	/*14*/	{"ana c", 0xa1, 0, NONE},
		{"ana d", 0xa2, 0, NONE},
	/*16*/	{"ana e", 0xa3, 0, NONE},
		{"ana h", 0xa4, 0, NONE},
	/*18*/	{"ana l", 0xa5, 0, NONE},
		{"ana m", 0xa6, 0, NONE},
	/*1a*/	{"ani", 0xe6, 1, D8},
		{"call", 0xcd, 2, ADDR},
	/*1c*/	{"cc", 0xdc, 2, ADDR},
		{"cm", 0xfc, 2, ADDR},
	/*1e*/	{"cma", 0x2f, 0, NONE},
		{"cmc", 0x3f, 0, NONE},
	/*20*/	{"cmp a", 0xbf, 0, NONE},
		{"cmp b", 0xb8, 0, NONE},
	/*22*/	{"cmp c", 0xb9, 0, NONE},
		{"cmp d", 0xba, 0, NONE},
	/*24*/	{"cmp e", 0xbb, 0, NONE},
		{"cmp h", 0xbc, 0, NONE},
	/*26*/	{"cmp l", 0xbd, 0, NONE},
		{"cmp m", 0xbe, 0, NONE},
	/*28*/	{"cnc", 0xd4, 2, ADDR},
		{"cnz", 0xc4, 2, ADDR},
	/*2a*/	{"cp", 0xf4, 2, ADDR},
		{"cpe", 0xec, 2, ADDR},
	/*2c*/	{"cpi", 0xfe, 1, D8},
		{"cpo", 0xe4, 2, ADDR},
	/*2e*/	{"cz", 0xcc, 2, ADDR},
		{"daa", 0x27, 0, NONE},
	/*30*/	{"dad b", 0x09, 0, NONE},
		{"dad d", 0x19, 0, NONE},
	/*32*/	{"dad h", 0x29, 0, NONE},
		{"dad sp", 0x39, 0, NONE},
	/*34*/	{"dcr a", 0x3d, 0, NONE},
		{"dcr b", 0x05, 0, NONE},
	/*36*/	{"dcr c", 0x0d, 0, NONE},
		{"dcr d", 0x15, 0, NONE},
	/*38*/	{"dcr e", 0x1d, 0, NONE},
		{"dcr h", 0x25, 0, NONE},
	/*3a*/	{"dcr l", 0x2d, 0, NONE},
		{"dcr m", 0x35, 0, NONE},
	/*3c*/	{"dcx b", 0x0b, 0, NONE},
		{"dcx d", 0x1b, 0, NONE},
	/*3e*/	{"dcx h", 0x2b, 0, NONE},
		{"dcx sp", 0x3b, 0, NONE},
	/*40*/	{"di", 0xf3, 0, NONE},
		{"ei", 0xfb, 0, NONE},
	/*42*/	{"hlt", 0x76, 0, NONE},
		{"in", 0xdb, 1, D8},
	/*44*/	{"inr a", 0x3c, 0, NONE},
		{"inr b", 0x04, 0, NONE},
	/*46*/	{"inr c", 0x0c, 0, NONE},
		{"inr d", 0x14, 0, NONE},
	/*48*/	{"inr e", 0x1c, 0, NONE},
		{"inr h", 0x24, 0, NONE},
	/*4a*/	{"inr l", 0x2c, 0, NONE},
		{"inr m", 0x34, 0, NONE},
	/*4c*/	{"inx b", 0x03, 0, NONE},
		{"inx d", 0x13, 0, NONE},
	/*4e*/	{"inx h", 0x23, 0, NONE},
		{"inx sp", 0x33, 0, NONE},
	/*50*/	{"jc", 0xda, 2, ADDR},
		{"jm", 0xfa, 2, ADDR},
	/*52*/	{"jmp", 0xc3, 2, ADDR},
		{"jnc", 0xd2, 2, ADDR},
	/*54*/	{"jnz", 0xc2, 2, ADDR},
		{"jp", 0xf2, 2, ADDR},
	/*56*/	{"jpe", 0xea, 2, ADDR},
		{"jpo", 0xe2, 2, ADDR},
	/*58*/	{"jz", 0xca, 2, ADDR},
		{"lda", 0x3a, 2, ADDR},
	/*5a*/	{"ldax b", 0x0a, 0, NONE},
		{"ldax d", 0x1a, 0, NONE},
	/*5c*/	{"lhld", 0x2a, 2, ADDR},
		{"lxi b", 0x01, 2, D16},
	/*5e*/	{"lxi d", 0x11, 2, D16},
		{"lxi h", 0x21, 2, D16},
	/*60*/	{"lxi sp", 0x31, 2, D16},
		{"mov a,a", 0x7f, 0, NONE},
	/*62*/	{"mov a,b", 0x78, 0, NONE},
		{"mov a,c", 0x79, 0, NONE},
	/*64*/	{"mov a,d", 0x7a, 0, NONE},
		{"mov a,e", 0x7b, 0, NONE},
	/*66*/	{"mov a,h", 0x7c, 0, NONE},
		{"mov a,l", 0x7d, 0, NONE},
	/*68*/	{"mov a,m", 0x7e, 0, NONE},
		{"mov b,a", 0x47, 0, NONE},
	/*6a*/	{"mov b,b", 0x40, 0, NONE},
		{"mov b,c", 0x41, 0, NONE},
	/*6c*/	{"mov b,d", 0x42, 0, NONE},
		{"mov b,e", 0x43, 0, NONE},
	/*6e*/	{"mov b,h", 0x44, 0, NONE},
		{"mov b,l", 0x45, 0, NONE},
	/*70*/	{"mov b,m", 0x46, 0, NONE},
		{"mov c,a", 0x4f, 0, NONE},
	/*72*/	{"mov c,b", 0x48, 0, NONE},
		{"mov c,c", 0x49, 0, NONE},
	/*74*/	{"mov c,d", 0x4a, 0, NONE},
		{"mov c,e", 0x4b, 0, NONE},
	/*76*/	{"mov c,h", 0x4c, 0, NONE},
		{"mov c,l", 0x4d, 0, NONE},
	/*78*/	{"mov c,m", 0x4e, 0, NONE},
		{"mov d,a", 0x57, 0, NONE},
	/*7a*/	{"mov d,b", 0x50, 0, NONE},
		{"mov d,c", 0x51, 0, NONE},
	/*7c*/	{"mov d,d", 0x52, 0, NONE},
		{"mov d,e", 0x53, 0, NONE},
	/*7e*/	{"mov d,h", 0x54, 0, NONE},
		{"mov d,l", 0x55, 0, NONE},
	/*80*/	{"mov d,m", 0x56, 0, NONE},
		{"mov e,a", 0x5f, 0, NONE},
	/*82*/	{"mov e,b", 0x58, 0, NONE},
		{"mov e,c", 0x59, 0, NONE},
	/*84*/	{"mov e,d", 0x5a, 0, NONE},
		{"mov e,e", 0x5b, 0, NONE},
	/*86*/	{"mov e,h", 0x5c, 0, NONE},
		{"mov e,l", 0x5d, 0, NONE},
	/*88*/	{"mov e,m", 0x5e, 0, NONE},
		{"mov h,a", 0x67, 0, NONE},
	/*8a*/	{"mov h,b", 0x60, 0, NONE},
		{"mov h,c", 0x61, 0, NONE},
	/*8c*/	{"mov h,d", 0x62, 0, NONE},
		{"mov h,e", 0x63, 0, NONE},
	/*8e*/	{"mov h,h", 0x64, 0, NONE},
		{"mov h,l", 0x65, 0, NONE},
	/*90*/	{"mov h,m", 0x66, 0, NONE},
		{"mov l,a", 0x6f, 0, NONE},
	/*92*/	{"mov l,b", 0x68, 0, NONE},
		{"mov l,c", 0x69, 0, NONE},
	/*94*/	{"mov l,d", 0x6a, 0, NONE},
		{"mov l,e", 0x6b, 0, NONE},
	/*96*/	{"mov l,h", 0x6c, 0, NONE},
		{"mov l,l", 0x6d, 0, NONE},
	/*98*/	{"mov l,m", 0x6e, 0, NONE},
		{"mov m,a", 0x77, 0, NONE},
	/*9a*/	{"mov m,b", 0x70, 0, NONE},
		{"mov m,c", 0x71, 0, NONE},
	/*9c*/	{"mov m,d", 0x72, 0, NONE},
		{"mov m,e", 0x73, 0, NONE},
	/*9e*/	{"mov m,h", 0x74, 0, NONE},
		{"mov m,l", 0x75, 0, NONE},
	/*a0*/	{"mvi a", 0x3e, 1, D8},
		{"mvi b", 0x06, 1, D8},
	/*a2*/	{"mvi c", 0x0e, 1, D8},
		{"mvi d", 0x16, 1, D8},
	/*a4*/	{"mvi e", 0x1e, 1, D8},
		{"mvi h", 0x26, 1, D8},
	/*a6*/	{"mvi l", 0x2e, 1, D8},
		{"mvi m", 0x36, 1, D8},
	/*a8*/	{"nop", 0x00, 0, NONE},
		{"ora a", 0xb7, 0, NONE},
	/*aa*/	{"ora b", 0xb0, 0, NONE},
		{"ora c", 0xb1, 0, NONE},
	/*ac*/	{"ora d", 0xb2, 0, NONE},
		{"ora e", 0xb3, 0, NONE},
	/*ae*/	{"ora h", 0xb4, 0, NONE},
		{"ora l", 0xb5, 0, NONE},
	/*b0*/	{"ora m", 0xb6, 0, NONE},
		{"ori", 0xf6, 1, D8},
	/*b2*/	{"out", 0xd3, 1, D8},
		{"pchl", 0xe9, 0, NONE},
	/*b4*/	{"pop b", 0xc1, 0, NONE},
		{"pop d", 0xd1, 0, NONE},
	/*b6*/	{"pop h", 0xe1, 0, NONE},
		{"pop psw", 0xf1, 0, NONE},
	/*b8*/	{"push b", 0xc5, 0, NONE},
		{"push d", 0xd5, 0, NONE},
	/*ba*/	{"push h", 0xe5, 0, NONE},
		{"push psw", 0xf5, 0, NONE},
	/*bc*/	{"ral", 0x17, 0, NONE},
		{"rar", 0x1f, 0, NONE},
	/*be*/	{"rc", 0xd8, 0, NONE},
		{"ret", 0xc9, 0, NONE},
	/*c0*/	{"rlc", 0x07, 0, NONE},
		{"rm", 0xf8, 0, NONE},
	/*c2*/	{"rnc", 0xd0, 0, NONE},
		{"rnz", 0xc0, 0, NONE},
	/*c4*/	{"rp", 0xf0, 0, NONE},
		{"rpe", 0xe8, 0, NONE},
	/*c6*/	{"rpo", 0xe0, 0, NONE},
		{"rrc", 0x0f, 0, NONE},
	/*c8*/	{"rst 0", 0xc7, 0, NONE},
		{"rst 1", 0xcf, 0, NONE},
	/*ca*/	{"rst 2", 0xd7, 0, NONE},
		{"rst 3", 0xdf, 0, NONE},
	/*cc*/	{"rst 4", 0xe7, 0, NONE},
		{"rst 5", 0xef, 0, NONE},
	/*ce*/	{"rst 6", 0xf7, 0, NONE},
		{"rst 7", 0xff, 0, NONE},
	/*d0*/	{"rz", 0xc8, 0, NONE},
		{"sbb a", 0x9f, 0, NONE},
	/*d2*/	{"sbb b", 0x98, 0, NONE},
		{"sbb c", 0x9a, 0, NONE},
	/*d4*/	{"sbb d", 0x9b, 0, NONE},
		{"sbb e", 0x9c, 0, NONE},
	/*d6*/	{"sbb h", 0x9d, 0, NONE},
		{"sbb l", 0x9e, 0, NONE},
	/*d8*/	{"sbb m", 0x9f, 0, NONE},
		{"sbi", 0xde, 1, D8},
	/*da*/	{"shld", 0x22, 2, ADDR},
		{"sphl", 0xf9, 0, NONE},
	/*dc*/	{"sta", 0x32, 2, ADDR},
		{"stax b", 0x02, 0, NONE},
	/*de*/	{"stax d", 0x12, 0, NONE},
		{"stc", 0x37, 0, NONE},
	/*e0*/	{"sub a", 0x97, 0, NONE},
		{"sub b", 0x90, 0, NONE},
	/*e2*/	{"sub c", 0x91, 0, NONE},
		{"sub d", 0x92, 0, NONE},
	/*e4*/	{"sub e", 0x93, 0, NONE},
		{"sub h", 0x94, 0, NONE},
	/*e6*/	{"sub l", 0x95, 0, NONE},
		{"sub m", 0x96, 0, NONE},
	/*e8*/	{"sui", 0xd6, 1, D8},
		{"xchg", 0xeb, 0, NONE},
	/*ea*/	{"xra a", 0xaf, 0, NONE},
		{"xra b", 0xa8, 0, NONE},
	/*ec*/	{"xra c", 0xa9, 0, NONE},
		{"xra d", 0xaa, 0, NONE},
	/*ee*/	{"xra e", 0xab, 0, NONE},
		{"xra h", 0xac, 0, NONE},
	/*f0*/	{"xra l", 0xad, 0, NONE},
		{"xra m", 0xae, 0, NONE},
	/*f2*/	{"xri", 0xee, 1, D8},
		{"xthl", 0xe3, 0, NONE}
};

//returns unused opcode (0x20) if no instruction is found in the line
instruction BinarySearch(char *line)
{
	int 	i,					
		result,					//indicates if binary search should go look ahead or behind of current search_index
		mnemonic_length,
		line_length = strlen(line),
		search_index = ARRAY_SIZE/2;		//index of instruction_set to check
	
	double rounded_shift_value = ARRAY_SIZE/2;	//helps to calculate next search index

	char* mnemonic;

	instruction no_instruction_found = {"no instruction found", 0x20, 0, NONE};

	while(search_index >= 0 && search_index <= ARRAY_SIZE - 1 && rounded_shift_value/2.0 >= 0.55)
	{
		mnemonic = instruction_set[search_index].mnemonic;
		mnemonic_length = strlen(mnemonic);

		if((result = strncmp(line, mnemonic, mnemonic_length)) == MATCH)
		{
			//if there is a match, we have to make sure it is not a case such as -> Match Candidate: JM, Expected: JMP	
			if(line_length > mnemonic_length)
			{
				if(line[mnemonic_length] < 'a' || line[mnemonic_length] > 'z')
				{
					//clear mnemonic from line
					for(i = 0; i < strlen(mnemonic); i++)
					{
						line[i] = ' ';
					}

					return instruction_set[search_index];
				}
			}
			else
			{
				//clear mnemonic from line
				for(i = 0; i < strlen(mnemonic); i++)
				{
					line[i] = ' ';
				}

				return instruction_set[search_index];
			}
		}

		//takes distance to shift away from current search_index, and adds 0.5 to prepare for rounding
		//rounding occurs during type conversion from double to int later on	
		rounded_shift_value = ((double)rounded_shift_value / 2.0) + 0.5;
	
		if(result < 0)
		{
			search_index = (int)((double)search_index - rounded_shift_value);
		}	
		else
		{
			search_index = (int)((double)search_index + rounded_shift_value);
		}		
	}

	return no_instruction_found;
}

