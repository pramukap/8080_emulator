/************************************************************************
 * 8080 Instruction Set							*
 * Pramuka Perera							*
 * June 23, 2017							*
 * Array of the 8080 instruction set, including a function pointer	*
 * to the function that emulates the function				* 
 ************************************************************************/

#ifndef INCLUDE
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

/*
 * Status Byte
 * FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
 * BIT   - 0     | 1         | 2    | 3    | 4 
 */
//r1 | r2 | rp1 | rp2 | name | size | flags | duration 
data instruction_set_data[INSTRUCTION_SET_SIZE] = 
{
	/*00*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4 },
		{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "LXI B, D16", 3, NONE, 10},
	/*02*/	{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "STAX B", 1, NONE, 7},
		{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "INX B", 1, NONE, 5},
	/*04*/	{register_file + B, NULL, NULL, "INR B", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + B, NULL, NULL, "DCR B", 1, ALL_EXCEPT_CARRY, 5},
	/*06*/ 	{NULL, register_file + B, NULL, "MVI B, D8", 2, NONE, 7},
	 	{NULL, NULL, NULL, "RLC", 1, CARRY, 4},
	/*08*/ 	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
	 	{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "DAD B", 1, CARRY, 10},
	/*0A*/ 	{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "LDAX B", 1, NONE, 7},
	 	{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "DCX B", 1, NONE, 5},
	/*0C*/ 	{register_file + C, NULL, NULL, "INR C", 1, ALL_EXCEPT_CARRY, 5},
	 	{register_file + C, NULL, NULL, "DCR C", 1, ALL_EXCEPT_CARRY, 5},
	/*0E*/	{NULL, register_file + C, NULL, "MVI C, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "RRC", 1, CARRY, 4},
	/*10*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "LXI D, D16", 3, NONE, 10},
	/*12*/	{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "STAX D", 1, NONE, 7},
		{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "INX D", 1, NONE, 5},
	/*14*/	{register_file + D, NULL, NULL, "INR D", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + D, NULL, NULL, "DCR D", 1, ALL_EXCEPT_CARRY, 5},
	/*16*/	{NULL, register_file + D, NULL, "MVI D, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "RAL", 1, CARRY, 4},
	/*18*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "DAD D", 1, CARRY, 10},
	/*1A*/	{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "LDAX D", 1, NONE, 7},
		{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "DCX D", 1, NONE, 5},
	/*1C*/	{register_file + E, NULL, NULL, "INR E", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + E, NULL, NULL, "DCR E", 1, ALL_EXCEPT_CARRY, 5},
	/*1E*/	{NULL, register_file + E, NULL, "MVI E, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "RAR", 1, CARRY, 4},
	/*20*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, (uint16_t *)(register_file + H_PAIR), "LXI H, D16", 3, NONE, 10},
	/*22*/	{NULL, NULL, NULL, "SHLD ADR", 3, NONE, 16},
		{NULL, NULL, (uint16_t *)(register_file + H_PAIR), "INX H", 1, NONE, 5},
	/*24*/	{register_file + H, NULL, NULL, "INR H", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + H, NULL, NULL, "DCR H", 1, ALL_EXCEPT_CARRY, 5},
	/*26*/	{NULL, register_file + H, NULL, "MVI H, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "DAA", 1, NONE, 4},
	/*28*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, (uint16_t *)(register_file + H_PAIR), "DAD H", 1, CARRY, 10},
	/*2A*/	{NULL, NULL, (uint16_t *)(register_file + H_PAIR), "LHLD ADR", 3, NONE, 16},
		{NULL, NULL, (uint16_t *)(register_file + H_PAIR), "DCX H", 1, NONE, 5},
	/*2C*/	{register_file + L, NULL, NULL, "INR L", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + L, NULL, NULL, "DCR L", 1, ALL_EXCEPT_CARRY, 5},
	/*2E*/	{NULL, register_file + L, NULL, "MVI L, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "CMA", 1, NONE, 4},
	/*30*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, &sp, "LXI SP, D16", 3, NONE, 10},
	/*32*/	{NULL, NULL, NULL, "STA ADR", 3, NONE, 13},
		{NULL, NULL, &sp, "INX SP", 1, NONE, 5},
	/*34*/	{NULL, NULL, NULL, "INR M", 1, ALL_EXCEPT_CARRY, 10},
		{NULL, NULL, NULL, "DCR M", 1, ALL_EXCEPT_CARRY, 10},
	/*36*/	{NULL, NULL, NULL, "MVI M, D8", 2, NONE, 10},
		{NULL, NULL, NULL, "STC", 1, CARRY, 4},
	/*38*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, &sp, "DAD SP", 1, CARRY, 10},
	/*3A*/	{NULL, NULL, NULL, "LDA ADR", 3, NONE, 13},
		{NULL, NULL, &sp, "DCX SP", 1, NONE, 5},
	/*3C*/	{register_file + A, NULL, NULL, "INR A", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + A, NULL, NULL, "DCR A", 1, ALL_EXCEPT_CARRY, 5},
	/*3E*/	{NULL, register_file + A, NULL, "MVI A, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "CMC", 1, CARRY, 4},
	/*40*/	{register_file + B, register_file + B, NULL, "MOV B, B", 1, NONE, 5},
		{register_file + C, register_file + B, NULL, "MOV B, C", 1, NONE, 5},
	/*42*/	{register_file + D, register_file + B, NULL, "MOV B, D", 1, NONE, 5},
		{register_file + E, register_file + B, NULL, "MOV B, E", 1, NONE, 5},
	/*44*/	{register_file + H, register_file + B, NULL, "MOV B, H", 1, NONE, 5},
		{register_file + L, register_file + B, NULL, "MOV B, L", 1, NONE, 5},
	/*46*/	{NULL, register_file + B, NULL, "MOV B, M", 1, NONE, 7},
		{register_file + A, register_file + B, NULL, "MOV B, A", 1, NONE, 5},
	/*48*/	{register_file + B, register_file + C, NULL, "MOV C, B", 1, NONE, 5},
		{register_file + C, register_file + C, NULL, "MOV C, C", 1, NONE, 5},
	/*4A*/	{register_file + D, register_file + C, NULL, "MOV C, D", 1, NONE, 5},
		{register_file + E, register_file + C, NULL, "MOV C, E", 1, NONE, 5},
	/*4C*/	{register_file + H, register_file + C, NULL, "MOV C, H", 1, NONE, 5},
		{register_file + L, register_file + C, NULL, "MOV C, L", 1, NONE, 5},
	/*4E*/	{NULL, register_file + C, NULL, "MOV C, M", 1, NONE, 7},
		{register_file + A, register_file + C, NULL, "MOV C, A", 1, NONE, 5},
	/*50*/	{register_file + B, register_file + D, NULL, "MOV D, B", 1, NONE, 5},
		{register_file + C, register_file + D, NULL, "MOV D, C", 1, NONE, 5},
	/*52*/	{register_file + D, register_file + D, NULL, "MOV D, D", 1, NONE, 5},
		{register_file + E, register_file + D, NULL, "MOV D, E", 1, NONE, 5},
	/*54*/	{register_file + H, register_file + D, NULL, "MOV D, H", 1, NONE, 5},
		{register_file + L, register_file + D, NULL, "MOV D, L", 1, NONE, 5},
	/*56*/	{NULL, register_file + D, NULL, "MOV D, M", 1, NONE, 7},
		{register_file + A, register_file + D, NULL, "MOV D, A", 1, NONE, 5},
	/*58*/	{register_file + B, register_file + E, NULL, "MOV E, B", 1, NONE, 5},
		{register_file + C, register_file + E, NULL, "MOV E, C", 1, NONE, 5},
	/*5A*/	{register_file + D, register_file + E, NULL, "MOV E, D", 1, NONE, 5},
		{register_file + E, register_file + E, NULL, "MOV E, E", 1, NONE, 5},
	/*5C*/	{register_file + H, register_file + E, NULL, "MOV E, H", 1, NONE, 5},
		{register_file + L, register_file + E, NULL, "MOV E, L", 1, NONE, 5},
	/*5E*/	{NULL, register_file + E, NULL, "MOV E, M", 1, NONE, 7},
		{register_file + A, register_file + E, NULL, "MOV E, A", 1, NONE, 5},
	/*60*/	{register_file + B, register_file + H, NULL, "MOV H, B", 1, NONE, 5},
		{register_file + C, register_file + H, NULL, "MOV H, C", 1, NONE, 5},
	/*62*/	{register_file + D, register_file + H, NULL, "MOV H, D", 1, NONE, 5},
		{register_file + E, register_file + H, NULL, "MOV H, E", 1, NONE, 5},
	/*64*/	{register_file + H, register_file + H, NULL, "MOV H, H", 1, NONE, 5},
		{register_file + L, register_file + H, NULL, "MOV H, L", 1, NONE, 5},
	/*66*/	{NULL, register_file + H, NULL, "MOV H, M", 1, NONE, 7},
		{register_file + A, register_file + H, NULL, "MOV H, A", 1, NONE, 5},
	/*68*/	{register_file + B, register_file + L, NULL, "MOV L, B", 1, NONE, 5},
		{register_file + C, register_file + L, NULL, "MOV L, C", 1, NONE, 5},
	/*6A*/	{register_file + D, register_file + L, NULL, "MOV L, D", 1, NONE, 5},
		{register_file + E, register_file + L, NULL, "MOV L, E", 1, NONE, 5},
	/*6C*/	{register_file + H, register_file + L, NULL, "MOV L, H", 1, NONE, 5},
		{register_file + L, register_file + L, NULL, "MOV L, L", 1, NONE, 5},
	/*6E*/	{NULL, register_file + L, NULL, "MOV L, M", 1, NONE, 7},
		{register_file + A, register_file + L, NULL, "MOV L, A", 1, NONE, 5},
	/*70*/	{register_file + B, NULL, NULL, "MOV M, B", 1, NONE, 7}, 
		{register_file + C, NULL, NULL, "MOV M, C", 1, NONE, 7}, 
	/*72*/	{register_file + D, NULL, NULL, "MOV M, D", 1, NONE, 7}, 
		{register_file + E, NULL, NULL, "MOV M, E", 1, NONE, 7}, 
	/*74*/	{register_file + H, NULL, NULL, "MOV M, H", 1, NONE, 7}, 
		{register_file + L, NULL, NULL, "MOV M, L", 1, NONE, 7}, 
	/*76*/	{NULL, NULL, NULL, "HLT", 1, NONE, 7}, 
		{register_file + A, NULL, NULL, "MOV M, A", 1, NONE, 7},
	/*78*/	{register_file + B, register_file + A, NULL, "MOV A, B", 1, NONE, 5},
		{register_file + C, register_file + A, NULL, "MOV A, C", 1, NONE, 5},
	/*7A*/	{register_file + D, register_file + A, NULL, "MOV A, D", 1, NONE, 5},
		{register_file + E, register_file + A, NULL, "MOV A, E", 1, NONE, 5},
	/*7C*/	{register_file + H, register_file + A, NULL, "MOV A, H", 1, NONE, 5},
		{register_file + L, register_file + A, NULL, "MOV A, L", 1, NONE, 5},
	/*7E*/	{NULL, register_file + A, NULL, "MOV A, M", 1, NONE, 7},
		{register_file + A, register_file + A, NULL, "MOV A, A", 1, NONE, 5},
 	/*80*/	{register_file + B, NULL, NULL, "ADD B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "ADD C", 1, ALL, 4},
	/*82*/	{register_file + D, NULL, NULL, "ADD D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "ADD E", 1, ALL, 4},
	/*84*/	{register_file + H, NULL, NULL, "ADD H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "ADD L", 1, ALL, 4},
	/*86*/	{NULL, NULL, NULL, "ADD M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "ADD A", 1, ALL, 4},
	/*88*/	{register_file + B, NULL, NULL, "ADC B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "ADC C", 1, ALL, 4},
	/*8A*/	{register_file + D, NULL, NULL, "ADC D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "ADC E", 1, ALL, 4},
	/*8C*/	{register_file + H, NULL, NULL, "ADC H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "ADC L", 1, ALL, 4},
	/*8E*/	{NULL, NULL, NULL, "ADC M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "ADC A", 1, ALL, 4},
	/*90*/	{register_file + B, NULL, NULL, "SUB B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "SUB C", 1, ALL, 4},
	/*92*/	{register_file + D, NULL, NULL, "SUB D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "SUB E", 1, ALL, 4},
	/*94*/	{register_file + H, NULL, NULL, "SUB H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "SUB L", 1, ALL, 4},
	/*96*/	{NULL, NULL, NULL, "SUB M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "SUB A", 1, ALL, 4},
	/*98*/	{register_file + B, NULL, NULL, "SBB B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "SBB C", 1, ALL, 4},
	/*9A*/	{register_file + D, NULL, NULL, "SBB D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "SBB E", 1, ALL, 4},
	/*9C*/	{register_file + H, NULL, NULL, "SBB H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "SBB L", 1, ALL, 4},
	/*9E*/	{NULL, NULL, NULL, "SBB M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "SBB A", 1, ALL, 4},
	/*A0*/	{register_file + B, NULL, NULL, "ANA B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "ANA C", 1, ALL, 4},
	/*A2*/	{register_file + D, NULL, NULL, "ANA D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "ANA E", 1, ALL, 4},
	/*A4*/	{register_file + H, NULL, NULL, "ANA H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "ANA L", 1, ALL, 4},
	/*A6*/	{NULL, NULL, NULL, "ANA M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "ANA A", 1, ALL, 4},
	/*A8*/	{register_file + B, NULL, NULL, "XRA B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "XRA C", 1, ALL, 4},
	/*AA*/	{register_file + D, NULL, NULL, "XRA D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "XRA E", 1, ALL, 4},
	/*AC*/	{register_file + H, NULL, NULL, "XRA H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "XRA L", 1, ALL, 4},
	/*AE*/	{NULL, NULL, NULL, "XRA M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "XRA A", 1, ALL, 4},
	/*B0*/	{register_file + B, NULL, NULL, "ORA B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "ORA C", 1, ALL, 4},
	/*B2*/	{register_file + D, NULL, NULL, "ORA D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "ORA E", 1, ALL, 4},
	/*B4*/	{register_file + H, NULL, NULL, "ORA H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "ORA L", 1, ALL, 4},
	/*B6*/	{NULL, NULL, NULL, "ORA M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "ORA A", 1, ALL, 4},
	/*B8*/	{register_file + B, NULL, NULL, "CMP B", 1, ALL, 4},
		{register_file + C, NULL, NULL, "CMP C", 1, ALL, 4},
	/*BA*/	{register_file + D, NULL, NULL, "CMP D", 1, ALL, 4},
		{register_file + E, NULL, NULL, "CMP E", 1, ALL, 4},
	/*BC*/	{register_file + H, NULL, NULL, "CMP H", 1, ALL, 4},
		{register_file + L, NULL, NULL, "CMP L", 1, ALL, 4},
	/*BE*/	{NULL, NULL, NULL, "CMP M", 1, ALL, 7},
		{register_file + A, NULL, NULL, "CMP A", 1, ALL, 4},
	/*C0*/	{NULL, NULL, NULL, "RNZ", 1, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "POP B", 1, NONE, 10},
	/*C2*/	{NULL, NULL, NULL, "JNZ ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "JMP ADR", 3, NONE, 10},
	/*C4*/	{NULL, NULL, NULL, "CNZ ADR", 3, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + B_PAIR), "PUSH B", 1, NONE, 11},
	/*C6*/	{NULL, NULL, NULL, "ADI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 0", 1, NONE, 11},
	/*C8*/	{NULL, NULL, NULL, "RZ", 1, NONE, 0xFF},
		{NULL, NULL, NULL, "RET", 1, NONE, 10},
	/*CA*/	{NULL, NULL, NULL, "JZ ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "NOP", 1, NONE, 4},
	/*CC*/	{NULL, NULL, NULL, "CZ ADR", 3, NONE, 0xFF},
		{NULL, NULL, NULL, "CALL ADR", 3, NONE, 17},
	/*CE*/	{NULL, NULL, NULL, "ACI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 1", 1, NONE, 11},
	/*D0*/	{NULL, NULL, NULL, "RNC", 1, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "POP D", 1, NONE, 10},
	/*D2*/	{NULL, NULL, NULL, "JNC ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "OUT D8", 2, NONE, 10},
	/*D4*/	{NULL, NULL, NULL, "CNC ADR", 3, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + D_PAIR), "PUSH D", 1, NONE, 11},
	/*D6*/	{NULL, NULL, NULL, "SUI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 2", 1, NONE, 11},
	/*D8*/	{NULL, NULL, NULL, "RC", 1, NONE, 0xFF},
		{NULL, NULL, NULL, "NOP", 1, NONE, 4},
	/*DA*/	{NULL, NULL, NULL, "JC ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "IN D8", 2, NONE, 10},
	/*DC*/	{NULL, NULL, NULL, "CC ADR", 3, NONE, 0xFF},
		{NULL, NULL, NULL, "NOP", 1, NONE, 4},
	/*DE*/	{NULL, NULL, NULL, "SBI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 3", 1, NONE, 11},
	/*E0*/	{NULL, NULL, NULL, "RPO", 1, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + H_PAIR), "POP H", 1, NONE, 10},
	/*E2*/	{NULL, NULL, NULL, "JPO ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "XTHL", 1, NONE, 18},
	/*E4*/	{NULL, NULL, NULL, "CPO ADR", 3, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + H_PAIR), "PUSH H", 1, NONE, 11},
	/*E6*/	{NULL, NULL, NULL, "ANI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 4", 1, NONE, 11},
	/*E8*/	{NULL, NULL, NULL, "RPE", 1, NONE, 0xFF},
		{NULL, NULL, NULL, "PCHL", 1, NONE, 5},
	/*EA*/	{NULL, NULL, NULL, "JPE ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "XCHG", 1, NONE, 4},
	/*EC*/	{NULL, NULL, NULL, "CPE ADR", 3, NONE, 0xFF},
		{NULL, NULL, NULL, "NOP", 1, NONE, 4},
	/*EE*/	{NULL, NULL, NULL, "XRI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 5", 1, NONE, 11},
	/*F0*/	{NULL, NULL, NULL, "RP", 1, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + PSW), "POP PSW", 1, NONE, 10},
	/*F2*/	{NULL, NULL, NULL, "JP ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "DI", 1, NONE, 4},
	/*F4*/	{NULL, NULL, NULL, "CP ADR", 3, NONE, 0xFF},
		{NULL, NULL, (uint16_t *)(register_file + PSW), "PUSH PSW", 1, NONE, 11},
	/*F6*/	{NULL, NULL, NULL, "ORI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 6", 1, NONE, 11},
	/*F8*/	{NULL, NULL, NULL, "RM", 1, NONE, 0xFF},
		{NULL, NULL, NULL, "SPHL", 1, NONE, 5},
	/*FA*/	{NULL, NULL, NULL, "JM ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "EI", 1, NONE, 4},
	/*FC*/	{NULL, NULL, NULL, "CM ADR", 3, NONE, 0xFF},
		{NULL, NULL, NULL, "NOP", 1, NONE, 4},
	/*FE*/	{NULL, NULL, NULL, "CPI D8", 2, ALL, 7},
		{NULL, NULL, NULL, "RST 7", 1, NONE, 11}	
};

//Instruction-Emulating Functions

//Special Emulator Functions
static inline void AddTime(uint8_t duration_of_instruction)
{
	//what to do for instructions with 2 timings

	time += duration_of_instruction;
}

static inline void OutputToDebugTerminal(char *instruction_name)
{
	printf("%s\n", instruction_name);
}

static inline void ModifyFlags(uint8_t bit_4_sum, uint16_t result, uint8_t flags_to_modify)
{
	uint8_t i, parity, _result, bit_4_result;

	//Modify Parity Flag (Set if even parity)
	if(flags_to_modify & 0x10)
	{
		_result = result;
		parity = 0;

		for(i = 0; i < 8; i++)
		{
			parity += _result & 0x01;
			_result >>= 1; 			
		}

		status[0] &= ~0x10;
		status[0] += !(parity % 2) << 4;
	}

	//Modify Zero Flag (Set if 0)
	if(flags_to_modify & 0x08)
	{
		if((uint8_t)result == 0)
		{
			status[0] |= 0x08;
		}
		else
		{
			status[0] &= ~0x08;
		}		
	}
	
	//Modify Sign Flag (Set if 1)
	if(flags_to_modify & 0x04)
	{
		status[0] &= ~0x04;
		status[0] += (result & 0x80) >> 5;
	}

	//Modify Aux Carry Flag (Set if Carry)
	if(flags_to_modify & 0x02)
	{

		bit_4_result = (result & 0x10) >> 4;
	
		/*
		 *The following cases show the result when there is no carry and when there is a carry
		 *Carry		0	0	0	0	1	1	1	1
		 *Operand 1  	0	0	1	1	0	0	1	1	
		 *Operand 2	0	1	0	1	0	1	0	1
		 *Result	0	1	1	0C1	1	0C1	0C1	1C1
		 */
		if(((bit_4_sum == 0 || bit_4_sum == 2) && bit_4_result == 1)
		|| (bit_4_sum == 1 && bit_4_result == 0))
		{
			status[0] |= 0x02;
		}
		else 
		{
			status[0] &= ~0x02;
		}
	}

	//Modify Carry/Borrow Flag (Set if Carry/Borrow)
	if(flags_to_modify & 0x01)
	{
		status[0] &= ~0x01;
		status[0] += (result & 0x100) >> 8; 		
	}
}

//Data Transfer
//Move contents of source register to destination register (0x40 to 0x7F excluding 0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E, 0x70-0x77)
void MovRegister(data *in)
{
	uint8_t *destination_register = in -> register_2, 
		*source_register = in -> register_1;
		
	*destination_register = *source_register;		
}

//Move to memory (0x70-0x77 excluding 0x76)
void MovToMemory(data *in)
{
	uint8_t *source = in -> register_1;
	uint16_t address = h_pair[0];		
	
	memory[address] = source[0];
}

//Move from memory (0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E, 0x7E)
void MovFromMemory(data *in)
{ 
	uint8_t *destination = in -> register_2;
	uint16_t address = h_pair[0];
	
	destination[0] = memory[address];	
}

//Move immediate value to register or memory (0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E) 
void Mvi(data *in)
{
	uint8_t *destination_is_register = in -> register_2;	
	uint16_t address = h_pair[0];

	if(destination_is_register)
	{
		destination_is_register[0] = memory[pc];
		pc += 1;
		
		return;	
	}

	memory[address] = memory[pc];
	pc += 1;
}

//Load immediate value to register pair (0x01, 0x11, 0x21, 0x31)
void Lxi(data *in)
{
	uint8_t high_byte, low_byte;
	uint16_t *destination_register_pair = in -> register_pair;

	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;

	destination_register_pair[0] = (high_byte << 8) + low_byte;
}

//Load Accumulator directly (0x3A)
void Lda(data *in)
{
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;	

	address = (high_byte << 8) + low_byte;
	
	a[0] = memory[address];
}

//Load Accumulator directly (0x32)
void Sta(data *in)
{
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;	

	address = (high_byte << 8) + low_byte;
	
	memory[address] = a[0];
}

//Load register pair H directly (0x2A)
void Lhld(data *in)
{
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;
	
	address = (high_byte << 8) + low_byte;
	
	l[0] = memory[address + 0];
	h[0] = memory[address + 1];
}

//Store register pair H directly (0x22)
void Shld(data *in)
{
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;
	
	address = (high_byte << 8) + low_byte;
	
	memory[address + 0] = l[0];
	memory[address + 1] = h[0];
}

//Load accumulator indirect (0x0A, 0x1A)
void Ldax(data *in)
{
	uint16_t address = (in -> register_pair)[0];	

	a[0] = memory[address];
}


//Store accumulator indirect (0x02, 0x12)
void Stax(data *in)
{
	uint16_t address = (in -> register_pair)[0];
	
	memory[address] = a[0];
}

//Exchange contents of register pair H with contents of register pair D (0xEB)
void Xchg(data *in)
{
	uint16_t temporary_register_pair = h_pair[0];

	h_pair[0] = d_pair[0];
	d_pair[0] = temporary_register_pair;
}

//Arithmetic
//Add contents of register to accumulator
void AddRegister(data *in)
{
	uint8_t addend = (in -> register_1)[0],
		bit_4_sum;
	uint16_t result; 

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((addend & MASK4) >> 4);
	result = a[0] + addend;

	a[0] += addend;

	ModifyFlags(bit_4_sum, result, in -> flags); 
}

//Subtract contents of register from accumulator
void SubRegister(data *in)
{
	uint8_t subtrahend = (in -> register_1)[0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((subtrahend & MASK4) >> 4);
	result = a[0] - subtrahend;

	a[0] -= subtrahend;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Add contents of memory to accumulator
void AddMemory(data *in)
{
	uint8_t addend = memory[h_pair[0]],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((addend & MASK4) >> 4);
	result = a[0] + addend;

	a[0] += addend;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Subtract contents of memory from accumulator
void SubMemory(data *in)
{	
	uint8_t subtrahend = memory[h_pair[0]],	
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((subtrahend & MASK4) >> 4);
	result = a[0] - subtrahend;

	a[0] -= subtrahend;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Add immediate
void Adi(data *in)
{
	uint8_t addend = memory[pc + 0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((addend & MASK4) >> 4);
	result = a[0] + addend;

	a[0] += addend;
	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Subtract immediate
void Sui(data *in)
{
	uint8_t subtrahend = memory[pc+0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((subtrahend & MASK4) >> 4);
	result = a[0] - subtrahend;

	a[0] -= subtrahend;
	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Add register with carry
void AdcRegister(data *in)
{
	uint8_t addend = (in -> register_1)[0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((addend & MASK4) >> 4);
	result = a[0] + addend;

	a[0] += addend + (status[0] & 0x01);	//status bit 0 is the carry/borrow flag	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Subtract register with borrow
void SbbRegister(data *in)
{
	uint8_t subtrahend = (in -> register_1)[0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((subtrahend & MASK4) >> 4);
	result = a[0] - subtrahend;

	a[0] -= (subtrahend + (status[0] & 0x01));

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Add memory with carry
void AdcMemory(data *in)
{
	uint8_t addend = memory[h_pair[0]],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((addend & MASK4) >> 4);
	result = a[0] + addend;

	a[0] += addend + (status[0] & 0x01);	//status bit 0 is the carry/borrow flag	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
}


//Subtract memory with borrow
void SbbMemory(data *in)
{
	uint8_t subtrahend = memory[h_pair[0]],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((subtrahend & MASK4) >> 4);
	result = a[0] - subtrahend;

	a[0] -= (subtrahend + (status[0] & 0x01));

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Add immediate with carry
void Aci(data *in)
{
	uint8_t addend = memory[pc + 0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((addend & MASK4) >> 4);
	result = a[0] + addend;

	a[0] += addend + (status[0] & 0x01);
	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Subtract immediate with borrow
void Sbi(data *in)
{
	uint8_t subtrahend = memory[pc + 0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((subtrahend & MASK4) >> 4);
	result = a[0] - subtrahend;

	a[0] -= (subtrahend + (status[0] & 0x01));
	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
}

//Increment register
void InrRegister(data *in)
{
	uint8_t *increment_register = (in -> register_1),
		bit_4;

	bit_4 = (increment_register[0] & MASK4) >> 4;

	increment_register[0] += 1;

	ModifyFlags(bit_4, (uint16_t)increment_register[0], in -> flags);
}

//Decrement register
void DcrRegister(data *in)
{
	uint8_t *decrement_register = (in -> register_1),
		bit_4;

	bit_4 = (decrement_register[0] & MASK4) >> 4;	

	decrement_register[0] -= 1;

	ModifyFlags(bit_4, (uint16_t)decrement_register[0], in -> flags);
}

//Increment memory
void InrMemory(data *in)
{
	uint8_t bit_4;
	uint16_t address = h_pair[0];

	bit_4 = (memory[address] & MASK4) >> 4;

	memory[address] += 1;

	ModifyFlags(bit_4, (uint16_t)memory[address], in -> flags);
}

//Decrement memory
void DcrMemory(data *in)
{
	uint8_t bit_4;
	uint16_t address = h_pair[0];

	bit_4 = (memory[address] & MASK4) >> 4;

	memory[address] -= 1;

	ModifyFlags(bit_4, (uint16_t)memory[address], in -> flags);
}

//Increment register pair
void Inx(data *in)
{
	uint16_t *increment_register_pair = in -> register_pair;

	increment_register_pair[0] += 1;
}

//Decrement register pair
void Dcx(data *in)
{
	uint16_t *decrement_register_pair = in -> register_pair;

	decrement_register_pair[0] -= 1;
}

//Add register pair to register pair H
void Dad(data *in)
{
	uint16_t addend = (in -> register_pair)[0],
		 result;

	result = h_pair[0] + addend;

	h_pair[0] += addend;

	ModifyFlags(0, result, in -> flags);
}

//Decimal Adjust Accumulator
void Daa(data *in)
{
	uint8_t bit_4;
	uint16_t result = a[0];

	bit_4 = ((a[0] & MASK4) >> 4);

	if((a[0] & 0x0F) > 9 || (status[0] & 0x02))		//status bit 1 is the auxiliary carry flag
	{
		result = a[0] + 0x06;
		a[0] += 0x06;
	} 

	if(((a[0] & 0xF0) >> 4) > 9 || (status[0] & 0x01))	//status bit 0 is the carry flag
	{
		result = a[0] +  0x60;
		a[0] += 0x60;
	} 

	ModifyFlags(bit_4, result, in -> flags);
}

//Logic
//AND register
void AnaRegister(data *in)
{
	uint8_t and_value = (in -> register_1)[0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((and_value & MASK4) >> 4);
	result = a[0] & and_value;

	a[0] &= and_value;

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x01;
}

//AND memory
void AnaMemory(data *in)
{
	uint8_t bit_4_sum;
	uint16_t address = h_pair[0],
		 result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((memory[address] & MASK4) >> 4);
	result = a[0] & memory[address];

	a[0] &= memory[address];

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x01;
}

//AND immediate
void Ani(data *in)
{
	uint8_t and_value = memory[pc + 0], 
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((and_value & MASK4) >> 4);	

	result = a[0] & and_value;

	a[0] &= and_value;
	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x03;
}

//XOR register
void XraRegister(data *in)
{
	uint8_t xor_value = (in -> register_1)[0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((xor_value & MASK4) >> 4);

	result = a[0] ^ xor_value;

	a[0] ^= xor_value;

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x03;
}

//XOR memory
void XraMemory(data *in)
{
	uint8_t bit_4_sum;
	uint16_t address = h_pair[0],
		 result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((memory[address] & MASK4) >> 4);

	result = a[0] ^ memory[address];

	a[0] ^= memory[address];
	
	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x03;
}

//XOR immediate
void Xri(data *in)
{
	uint8_t xor_value = memory[pc + 0],
  		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((xor_value & MASK4) >> 4);

	result = a[0] ^ xor_value;

	a[0] ^= xor_value;
	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x03;
}

//OR register
void OraRegister(data *in)
{
	uint8_t or_value = (in -> register_1)[0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((or_value & MASK4) >> 4);

	result = a[0] | or_value;

	a[0] |= or_value;

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x03;
}

//OR memory
void OraMemory(data *in)
{
	uint8_t bit_4_sum;
	uint16_t address = h_pair[0],
		 result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((memory[address] & MASK4) >> 4);

	result = a[0] | memory[address];

	a[0] |= memory[address];

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x03;
}

//OR immediate
void Ori(data *in)
{
	uint8_t or_value = memory[pc + 0],
		bit_4_sum;
	uint16_t result;

	bit_4_sum = ((a[0] & MASK4) >> 4) + ((or_value & MASK4) >> 4);

	result = a[0] | or_value;

	a[0] |= or_value;
	pc += 1;

	ModifyFlags(bit_4_sum, result, in -> flags);
	status[0] &= ~0x03;
}

//Compare register
void CmpRegister(data *in)
{
	uint8_t compare_value = (in -> register_1)[0],
		bit_4_sum = ((a[0] & MASK4) >> 4) + ((compare_value & MASK4) >> 4),
		_status = status[0];
	uint16_t result = a[0] - compare_value;
	
	ModifyFlags(bit_4_sum, result, in -> flags);

	/*
	 *restore the original values of Z and CY because 
	 *ModifyFlags changes them improperly and 
	 *the compare instruction is not guaranteed to change them
	 */
	status[0] &= ~(Z + CY);
	status[0] += _status & (Z + CY);

	if(a[0] == compare_value)
	{
		status[0] |= 0x08;	
	}

	if(a[0] < compare_value)
	{
		status[0] |= 0x01;
	}
}

//Compare memory
void CmpMemory(data *in)
{
	uint16_t address = h_pair[0],
		 result = a[0] - memory[address];
	uint8_t bit_4_sum = ((a[0] & MASK4) >> 4) + ((memory[address] & MASK4) >> 4),
		_status = status[0];

	ModifyFlags(bit_4_sum, result, in -> flags);

	/*
	 *restore the original values of Z and CY because 
	 *ModifyFlags changes them improperly and 
	 *the compare instruction is not guaranteed to change them
	 */
	status[0] &= ~(Z + CY);
	status[0] += _status & (Z + CY);

	if(a[0] == memory[address])
	{
		status[0] |= 0x08;	
	}

	if(a[0] < memory[address])
	{
		status[0] |= 0x01;
	}
}

//Compare immediate
void Cpi(data *in)
{
	uint8_t compare_value = memory[pc + 0],
		bit_4_sum = ((a[0] & MASK4) >> 4) + ((compare_value & MASK4) >> 4),
		_status = status[0];
	uint16_t result = a[0] - compare_value;
	
	ModifyFlags(bit_4_sum, result, in -> flags);

	/*
	 *restore the original values of Z and CY because 
	 *ModifyFlags changes them improperly and 
	 *the compare instruction is not guaranteed to change them
	 */
	status[0] &= ~(Z + CY);
	status[0] += _status & (Z + CY);

	if(a[0] == compare_value)
	{
		status[0] |= 0x08;	
	}

	if(a[0] < compare_value)
	{
		status[0] |= 0x01;
	}
	
	pc += 1;
}

//Rotate left
void Rlc(data *in)
{
	uint8_t bit_7 = (a[0] & 0x80) >> 7;

	a[0] = (a[0] << 1) + bit_7;
	
	status[0] &= ~CY;
	status[0] += bit_7;
}

//Rotate right
void Rrc(data *in)
{
	uint8_t bit_0 = a[0] & 0x01;

	a[0] = (a[0] >> 1) + (bit_0 << 7);

	status[0] &= ~CY;
	status[0] += bit_0;
}

//Rotate left through carry
void Ral(data *in)
{
	uint8_t new_value_of_carry = (a[0] * 0x80) >> 7;

	a[0] = (a[0] << 1) + (status[0] & 0x01);			//pass current carry flag value to bit 0 of accumulator
	status[0] = (status[0] & ~CY) + new_value_of_carry;		//pass bit 7 of accumulator to carry flag
}

//Rotate right through carry
void Rar(data *in)
{
	uint8_t new_value_of_carry = a[0] & 0x01;

	a[0] = (a[0] >> 1) + ((status[0] & 0x01) << 7);
	status[0] = (status[0] & ~CY) + new_value_of_carry;
}

//Complement accumulator
void Cma(data *in)
{
	a[0] = ~a[0];
}

//Complement carry
void Cmc(data *in)
{
	status[0] ^= 0x01;
}
//Set carry
void Stc(data *in)
{
	status[0] |= 0x01;
}

//Branch
//Unconditional jump
void Jmp(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];
	
	uint16_t address = (high_byte << 8) + low_byte;

	pc = address;
}

//Conditional jumps
void Jnz(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];
	
	uint16_t address = (high_byte << 8) + low_byte;

	if(!(status[0] & 0x08))
	{
		pc = address;
		return;
	}

	pc += 2;
}

void Jz(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];
	
	uint16_t address = (high_byte << 8) + low_byte;

	if(status[0] & 0x08)
	{
		pc = address;
		return;
	}

	pc += 2;
}

void Jnc(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];
	
	uint16_t address = (high_byte << 8) + low_byte;

	if(!(status[0] & 0x01))
	{
		pc = address;
		return;
	}

	pc += 2;
}

void Jc(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];
	
	uint16_t address = (high_byte << 8) + low_byte;

	if(status[0] & 0x01)
	{
		pc = address;
		return;
	}

	pc += 2;
}

void Jpo(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;

	if(!(status[0] & 0x10))
	{
		pc = address;
		return;
	}

	pc += 2;
}

void Jpe(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;

	if(status[0] & 0x10)
	{
		pc = address;
		return;
	}

	pc += 2;
}

void Jp(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;

	if(!(status[0] & 0x04))
	{
		pc = address;
		return;
	}

	pc += 2;
}

void Jm(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];
	
	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x04)
	{
		pc = address;
		return;
	}
	
	pc += 2;
}

//Unconditional call
void Call(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	pc += 2;	
	sp -= 2;		
	memory[sp] = pc;
	pc = address;
}

//Conditional calls
void Cnz(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x08))
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

void Cz(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x08)
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

void Cnc(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x01))
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

void Cc(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x01)
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

void Cpo(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x10))
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

void Cpe(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x10)
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

void Cp(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x04))
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

void Cm(data *in)
{
	uint8_t high_byte = memory[pc + 1],
		low_byte = memory[pc + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x04)
	{
		pc += 2;	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

//Unconditional return
void Ret(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	sp += 2;
	pc = address;
}

//Conditional returns
void Rnz(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x08))
	{
		sp += 2;
		pc = address;
	}
}

void Rz(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x08)
	{
		sp += 2;
		pc = address;
	}
}

void Rnc(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x01))
	{
		sp += 2;
		pc = address;	
	}
}

void Rc(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x01)
	{
		sp += 2;
		pc = address;
	}
}

void Rpo(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x10))
	{
		sp += 2;
		pc = address;
	}
}

void Rpe(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x10)
	{
		sp += 2;
		pc = address;
	}
}

void Rp(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(!(status[0] & 0x04))
	{
		sp += 2;
		pc = address;
	}
}

void Rm(data *in)
{
	uint8_t high_byte = memory[sp + 1],
		low_byte = memory[sp + 0];

	uint16_t address = (high_byte << 8) + low_byte;
	
	if(status[0] & 0x04)
	{
		sp += 2;
		pc = address;
	}
}

//Restart
void Rst(data *in)
{
	uint16_t address = ((instruction_register & 0x38) >> 3) * 8;
	
	if(status[0] & 0x04)
	{	
		sp -= 2;		
		memory[sp] = pc;
		pc = address;
	}
}

//Move register pair H to pc
void Pchl(data *in)
{
	pc = h_pair[0];
}

//Stack, IO, Machine Control
//Push register
void PushRp(data *in)
{
	uint16_t pushed_value = (in -> register_pair)[0];

	sp -= 2;
	memory[sp] = pushed_value;
}

//Push psw
void PushPsw(data *in)
{
	uint8_t pushed_status = 0;

	pushed_status = ((status[0] & 0x10) >> 2) + 
			((status[0] & 0x08) << 3) + 
			((status[0] & 0x04) << 5) +
			((status[0] & 0x02) << 3);	

	memory[sp - 1] = a[0];
	memory[sp - 2] = pushed_status;
	
	sp -= 2;
}

//Pop register
void PopRp(data *in)
{
	uint16_t *destination_register_pair = (in -> register_pair);
	
	destination_register_pair[0] = (uint16_t)memory[sp];
	
	sp += 2;
}

//Pop psw
void PopPsw(data *in)
{
	uint8_t popped_status = memory[sp + 0];

	status[0] = 	((popped_status & 0x80) >> 5) +
			((popped_status & 0x40) >> 3) +
			((popped_status & 0x10) >> 3) +
			((popped_status & 0x04) << 2);

	a[0] = memory[sp + 1];

	sp += 2;
}

//Exchange top two bytes on stack with register pair H
void Xthl(data *in)
{
	uint8_t temporary_h_register = h[0],
		temporary_l_register = l[0];

	h_pair[0] = (uint16_t)memory[sp];
	
	memory[sp + 0] = temporary_l_register;
	memory[sp + 1] = temporary_h_register;
}

//Set sp to register pair H
void Sphl(data *in)
{
	sp = h_pair[0];
}

//Input
void In(data *in)
{
	uint8_t port = memory[pc + 0];
	pc += 1;

	a[0] = io[port];
}

//Output
void Out(data *in)
{
	uint8_t port = memory[pc + 0];
	pc += 1;

	io[port] = a[0];
}

//Enable interrupts
void Ei(data *in)
{
	interrupt_enable |= 0x01;
}

//Disable interrupts
void Di(data *in)
{
	interrupt_enable &= ~0x01;
}

//Halt
void Hlt(data *in)
{
	halt_enable |= 0x01;
}

//Nop
void Nop(data *in)
{

}

instruction instruction_set[INSTRUCTION_SET_SIZE] =
{
	/*00*/	Nop,
		Lxi,	
	/*02*/	Stax,
		Inx,
	/*04*/	InrRegister,
		DcrRegister,
	/*06*/	Mvi,
		Rlc,
	/*08*/	Nop,
		Dad,
	/*0A*/	Ldax,
		Dcx,
	/*0C*/	InrRegister,
		DcrRegister,
	/*0E*/	Mvi,
		Rrc,
	/*10*/	Nop,
		Lxi,
	/*12*/	Stax,
		Inx,
	/*14*/	InrRegister,
		DcrRegister,
	/*16*/	Mvi,
		Ral,
	/*18*/	Nop,
		Dad,
	/*1A*/	Ldax,
		Dcx,
	/*1C*/	InrRegister,
		DcrRegister,
	/*1E*/	Mvi,
		Rar,
	/*20*/	Nop,
		Lxi,	
	/*22*/	Shld,
		Inx,
	/*24*/	InrRegister,
		DcrRegister,
	/*26*/	Mvi,
		Daa,
	/*28*/	Nop,
		Dad,
	/*2A*/	Lhld,
		Dcx,
	/*2C*/	InrRegister,
		DcrRegister,
	/*2E*/	Mvi,
		Cma,
	/*30*/	Nop,
		Lxi,	
	/*32*/	Sta,
		Inx,
	/*34*/	InrMemory,
		DcrMemory,
	/*36*/	Mvi,
		Stc,
	/*38*/	Nop,
		Dad,
	/*3A*/	Lda,
		Dcx,
	/*3C*/	InrRegister,
		DcrRegister,
	/*3E*/	Mvi,
		Cmc,
	/*40*/	MovRegister,
		MovRegister,
	/*42*/	MovRegister,
		MovRegister,
	/*44*/	MovRegister,
		MovRegister,
	/*46*/	MovFromMemory,
		MovRegister,
	/*48*/	MovRegister,
		MovRegister,
	/*4A*/	MovRegister,
		MovRegister,
	/*4C*/	MovRegister,
		MovRegister,
	/*4E*/	MovFromMemory,
		MovRegister,
	/*50*/	MovRegister,
		MovRegister,
	/*52*/	MovRegister,
		MovRegister,
	/*54*/	MovRegister,
		MovRegister,
	/*56*/	MovFromMemory,
		MovRegister,
	/*58*/	MovRegister,
		MovRegister,
	/*5A*/	MovRegister,
		MovRegister,
	/*5C*/	MovRegister,
		MovRegister,
	/*5E*/	MovFromMemory,
		MovRegister,
	/*60*/	MovRegister,
		MovRegister,
	/*62*/	MovRegister,
		MovRegister,
	/*64*/	MovRegister,
		MovRegister,
	/*66*/	MovFromMemory,
		MovRegister,
	/*68*/	MovRegister,
		MovRegister,
	/*6A*/	MovRegister,
		MovRegister,
	/*6C*/	MovRegister,
		MovRegister,
	/*6E*/	MovFromMemory,
		MovRegister,
	/*70*/	MovToMemory,
		MovToMemory,
	/*72*/	MovToMemory,
		MovToMemory,
	/*74*/	MovToMemory,
		MovToMemory,
	/*76*/	Hlt,
		MovRegister,
	/*78*/	MovRegister,
		MovRegister,
	/*7A*/	MovRegister,
		MovRegister,
	/*7C*/	MovRegister,
		MovRegister,
	/*7E*/	MovFromMemory,
		MovRegister,
	/*80*/	AddRegister,
		AddRegister,
	/*82*/	AddRegister,
		AddRegister,
	/*84*/	AddRegister,
		AddRegister,
	/*86*/	AddMemory,
		AddRegister,
	/*88*/	AdcRegister,
		AdcRegister,
	/*8A*/	AdcRegister,
		AdcRegister,
	/*8C*/	AdcRegister,
		AdcRegister,
	/*8E*/	AdcMemory,
		AdcRegister,
	/*90*/	SubRegister,
		SubRegister,
	/*92*/	SubRegister,
		SubRegister,
	/*94*/	SubRegister,
		SubRegister,
	/*96*/	SubMemory,
		SubRegister,
	/*98*/	SbbRegister,
		SbbRegister,
	/*9A*/	SbbRegister,
		SbbRegister,
	/*9C*/	SbbRegister,
		SbbRegister,
	/*9E*/	SbbMemory,
		SbbRegister,
	/*A0*/	AnaRegister,
		AnaRegister,
	/*A2*/	AnaRegister,
		AnaRegister,
	/*A4*/	AnaRegister,
		AnaRegister,
	/*A6*/	AnaMemory,
		AnaRegister,
	/*A8*/	XraRegister,
		XraRegister,
	/*AA*/	XraRegister,
		XraRegister,
	/*AC*/	XraRegister,
		XraRegister,
	/*AE*/	XraMemory,
		XraRegister,
	/*B0*/	OraRegister,
		OraRegister,
	/*B2*/	OraRegister,
		OraRegister,
	/*B4*/	OraRegister,
		OraRegister,
	/*B6*/	OraMemory,
		OraRegister,
	/*B8*/	CmpRegister,
		CmpRegister,
	/*BA*/	CmpRegister,
		CmpRegister,
	/*BC*/	CmpRegister,
		CmpRegister,
	/*BE*/	CmpMemory,
		CmpRegister,
	/*C0*/	Rnz,
		PopRp,
	/*C2*/	Jnz,
		Jmp,
	/*C4*/	Cnz,
		PushRp,
	/*C6*/	Adi,
		Rst,
	/*C8*/	Rz,
		Ret,
	/*CA*/	Jz,
		Nop,
	/*CC*/	Cz,
		Call,
	/*CE*/	Aci,
		Rst,
	/*D0*/	Rnc,
		PopRp,
	/*D2*/	Jnc,
		Out,
	/*D4*/	Cnc,
		PushRp,
	/*D6*/	Sui,
		Rst,
	/*D8*/	Rc,
		Nop,
	/*DA*/	Jc,
		In,
	/*DC*/	Cc,
		Nop,
	/*DE*/	Sbi,
		Rst,
	/*E0*/	Rpo,
		PopRp,
	/*E2*/	Jpo,
		Xthl,
	/*E4*/	Cpo,
		PushRp,
	/*E6*/	Ani,
		Rst,
	/*E8*/	Rpe,
		Pchl,
	/*EA*/	Jpe,
		Xchg,
	/*EC*/	Cpe,
		Nop,
	/*EE*/	Xri,
		Rst,
	/*F0*/	Rp,
		PopPsw,
	/*F2*/	Jp,
		Di,
	/*F4*/	Cp,
		PushPsw,
	/*F6*/	Ori,
		Rst,
	/*F8*/	Rm,
		Sphl,
	/*FA*/	Jm,
		Ei,
	/*FC*/	Cm,
		Nop,
	/*FE*/	Cpi,
		Rst
};
