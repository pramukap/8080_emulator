/************************************************************************
 * 8080_Emulator							*
 * Pramuka Perera							*
 * June 23, 2017							*
 * Emulator for the Intel 8080 processor as used in the Altair 8800 	*
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define C			0
#define B			1
#define E			2
#define D			3
#define L			4
#define H			5
#define A			6
#define STATUS			7
#define Z			8
#define W			9

#define B_PAIR			0
#define D_PAIR			2
#define H_PAIR			4
#define PSW			6

#define MEMORY_SIZE		4000
#define ALL			0b00011111
#define NONE			0b00000000
#define ALL_EXCEPT_CARRY	0b00011110
#define CARRY			0b00000001

//HARDWARE---
static uint8_t *memory;
 
static uint8_t register_file[10];

/*
 * General Purpose Registers
 * x86 stores data using little endian format.
 *
 * These registers are accessed as register pairs (where B, D, and H hold the higher order byte) 
 * using  a uint16_t pointer in various instructions.
 *
 * As a result of these two facts, the registers are described in an order where the low order 
 * register is assigned a lower address than it's higher order counterpart.
 */
static uint8_t *const c = register_file + C;
static uint8_t *const b = register_file + B;
static uint8_t *const e = register_file + E;
static uint8_t *const d = register_file + D;
static uint8_t *const l = register_file + L;
static uint8_t *const h = register_file + H;

//Accumulator
static uint8_t *const a = register_file + A;

/*
 * Status Byte
 * FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
 * BIT   - 0     | 1         | 2    | 3    | 4 
 */
static uint8_t *const status = register_file + STATUS;

//Registers Z and W can only be used for instruction execution
//These registers are not directly accessible to the programmer
static uint8_t *const z = register_file + Z;
static uint8_t *const w = register_file + W;

//Register Pairs
//B+C
static uint16_t *const b_pair = (uint16_t*)(register_file + B_PAIR);
//D+E
static uint16_t *const d_pair = (uint16_t*)(register_file + D_PAIR);
//H+L
static uint16_t *const h_pair = (uint16_t*)(register_file + H_PAIR);
//A+status
static uint16_t *const psw = (uint16_t*)(register_file + PSW);

/*
 * The pc and sp contain the emulated address and not the host address.
 * The emulated address is in fact an index for the emulated memory,
 * which is a dynamically-allocated array.
 */
static uint16_t pc;
static uint16_t sp;

//Instruction Register
static uint8_t instruction_register;

//Processor Time
static uint32_t time;
//---

//I/O---
//Control I/O Signals of 8080
//SIGNALS - WR'(0) | DBIN(O) | INTE(O) | INT(I) | HOLD ACK(O) | HOLD(I) | WAIT (0) | READY(I) | SYNC(O) | RESET(I) 
//BIT     - 0      | 1       | 2       | 3      | 4           | 5       | 6        | 7        | 8       | 9
static uint16_t control;
//---

//USER INTERFACE---
//Signals presented on the 8800 front panel
//SIGNALS - INTE | PROT | MEMR | INP | M1 | OUT | HLTA | STACK | WO'  | INT(A) | WAIT | HLDA | RESET
//BIT     - 0    | 1    | 2    | 3   | 4  | 5   | 6    | 7     | 8    | 9      | 10   | 11   | 12
static uint16_t indicator;
//---

//8080 INSTRUCTION SET---
//Stores data relevant to instruction; accessed by instruction-emulating function
typedef struct instruction_data 
{
	uint8_t *const register_1;	//source register or only register mentioned in instruction
	uint8_t *const register_2;	//destination register
	uint16_t *const register_pair; 	//source or destination register pair
	const char name[12];		//Instruction mnemonic		
	const uint8_t size;		//size of instruction in bytes
	const uint8_t flags;		//flags triggered by instruction
	const uint8_t duration;		//number of clock cycles instruction takes (instructions marked that have 0xFF have (11 or 17) cc or (5 or 11) cc
} data; 

//Function that emulates instruction
typedef void (*instruction)(data *input); 

/*
 * Status Byte
 * FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
 * BIT   - 0     | 1         | 2    | 3    | 4 
 */
//r1 | r2 | rp1 | rp2 | name | size | flags | duration 
static data instruction_set_data[256] = 
{
	/*00*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4 },
		{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "LXI B, D16", 3, NONE, 10},
	/*02*/	{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "STAX B", 1, NONE, 7},
		{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "INX B", 1, NONE, 5},
	/*04*/	{register_file + B, NULL, NULL, "INR B", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + B, NULL, NULL, "DCR B", 1, ALL_EXCEPT_CARRY, 5},
	/*06*/ 	{register_file + B, NULL, NULL, "MVI B, D8", 2, NONE, 7},
	 	{NULL, NULL, NULL, "RLC", 1, CARRY, 4},
	/*08*/ 	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
	 	{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "DAD B", 1, CARRY, 10},
	/*0A*/ 	{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "LDAX B", 1, NONE, 7},
	 	{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "DCX B", 1, NONE, 5},
	/*0C*/ 	{register_file + C, NULL, NULL, "INR C", 1, ALL_EXCEPT_CARRY, 5},
	 	{register_file + C, NULL, NULL, "DCR C", 1, ALL_EXCEPT_CARRY, 5},
	/*0E*/	{register_file + C, NULL, NULL, "MVI C, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "RRC", 1, CARRY, 4},
	/*10*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "LXI D, D16", 3, NONE, 10},
	/*12*/	{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "STAX D", 1, NONE, 7},
		{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "INX D", 1, NONE, 5},
	/*14*/	{register_file + D, NULL, NULL, "INR D", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + D, NULL, NULL, "DCR D", 1, ALL_EXCEPT_CARRY, 5},
	/*16*/	{register_file + D, NULL, NULL, "MVI D, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "RAL", 1, CARRY, 4},
	/*18*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "DAD D", 1, CARRY, 10},
	/*1A*/	{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "LDAX D", 1, NONE, 7},
		{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "DCX D", 1, NONE, 5},
	/*1C*/	{register_file + E, NULL, NULL, "INR E", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + E, NULL, NULL, "DCR E", 1, ALL_EXCEPT_CARRY, 5},
	/*1E*/	{register_file + E, NULL, NULL, "MVI E, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "RAR", 1, CARRY, 4},
	/*20*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, ((uint16_t *)register_file) + H_PAIR, "LXI H, D16", 3, NONE, 10},
	/*22*/	{NULL, NULL, NULL, "SHLD ADR", 3, NONE, 16},
		{NULL, NULL, ((uint16_t *)register_file) + H_PAIR, "INX H", 1, NONE, 5},
	/*24*/	{register_file + H, NULL, NULL, "INR H", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + H, NULL, NULL, "DCR H", 1, ALL_EXCEPT_CARRY, 5},
	/*26*/	{register_file + H, NULL, NULL, "MVI H, D8", 2, NONE, 7},
		{NULL, NULL, NULL, "DAA", 1, NONE, 4},
	/*28*/	{NULL, NULL, NULL, "NOP", 1, NONE, 4},
		{NULL, NULL, ((uint16_t *)register_file) + H_PAIR, "DAD H", 1, CARRY, 10},
	/*2A*/	{NULL, NULL, ((uint16_t *)register_file) + H_PAIR, "LHLD ADR", 3, NONE, 16},
		{NULL, NULL, ((uint16_t *)register_file) + H_PAIR, "DCX H", 1, NONE, 5},
	/*2C*/	{register_file + L, NULL, NULL, "INR L", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + L, NULL, NULL, "DCR L", 1, ALL_EXCEPT_CARRY, 5},
	/*2E*/	{register_file + L, NULL, NULL, "MVI L, D8", 2, NONE, 7},
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
	/*3A*/	{NULL, NULL, NULL, "LDA adr", 3, NONE, 13},
		{NULL, NULL, &sp, "DCX SP", 1, NONE, 5},
	/*3C*/	{register_file + A, NULL, NULL, "INR A", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + A, NULL, NULL, "DCR A", 1, ALL_EXCEPT_CARRY, 5},
	/*3E*/	{register_file + A, NULL, NULL, "MVI A, D8", 2, NONE, 7},
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
		{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "POP B", 1, NONE, 10},
	/*C2*/	{NULL, NULL, NULL, "JNZ ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "JMP ADR", 3, NONE, 10},
	/*C4*/	{NULL, NULL, NULL, "CNZ ADR", 3, NONE, 0xFF},
		{NULL, NULL, ((uint16_t *)register_file) + B_PAIR, "PUSH B", 1, NONE, 11},
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
		{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "POP D", 1, NONE, 10},
	/*D2*/	{NULL, NULL, NULL, "JNC ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "OUT D8", 2, NONE, 10},
	/*D4*/	{NULL, NULL, NULL, "CNC ADR", 3, NONE, 0xFF},
		{NULL, NULL, ((uint16_t *)register_file) + D_PAIR, "PUSH D", 1, NONE, 11},
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
		{NULL, NULL, ((uint16_t *)register_file) + H_PAIR, "POP H", 1, NONE, 10},
	/*E2*/	{NULL, NULL, NULL, "JPO ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "XTHL", 1, NONE, 18},
	/*E4*/	{NULL, NULL, NULL, "CPO ADR", 3, NONE, 0xFF},
		{NULL, NULL, ((uint16_t *)register_file) + H_PAIR, "PUSH H", 1, NONE, 11},
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
		{NULL, NULL, ((uint16_t *)register_file) + PSW, "POP PSW", 1, NONE, 10},
	/*F2*/	{NULL, NULL, NULL, "JP ADR", 3, NONE, 10},
		{NULL, NULL, NULL, "DI", 1, NONE, 4},
	/*F4*/	{NULL, NULL, NULL, "CP ADR", 3, NONE, 0xFF},
		{NULL, NULL, ((uint16_t *)register_file) + PSW, "PUSH PSW", 1, NONE, 11},
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

static inline void AddTime(uint8_t duration_of_instruction)
{
	//what to do for instructions with 2 timings

	time += duration_of_instruction;
}

static inline void OutputToDebugTerminal(char *instruction_name)
{
	printf("%s\n", instruction_name);
}

//Data Transfer
//Move contents of source register to destination register (0x40 to 0x7F excluding 0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E, 0x70-0x77)
void MovRegister(data *input)
{
	uint8_t *destination_register = input -> register_2, 
		*source_register = input -> register_1;
		
	*destination_register = *source_register;		
}

//Move to memory (0x70-0x77 excluding 0x76)
void MovToMemory(data *input)
{
	uint8_t *source = input -> register_1;
	uint16_t address = h_pair[0];		
	
	memory[address] = source[0];
}

//Move from memory (0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E, 0x7E)
void MovFromMemory(data *input)
{ 
	uint8_t *destination = input -> register_2;
	uint16_t address = h_pair[0];
	
	destination[0] = memory[address];	
}

//Move immediate value to register or memory (0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E) 
void Mvi(data *input)
{
	uint16_t address = h_pair[0];
	uint8_t *destination_is_register = input -> register_2;	

	if(destination_is_register)
	{
		destination_is_register[0] = memory[pc];
		pc += ((input -> size) - 1);
		
		return;	
	}

	memory[address] = memory[pc];
	pc += ((input -> size) - 1);
}

//Load immediate value to register pair (0x01, 0x11, 0x21, 0x31)
void Lxi(data *input)
{
	uint8_t high_byte, low_byte;
	uint16_t *destination_register_pair = input -> register_pair;

	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;

	destination_register_pair[0] = (high_byte << 8) + low_byte;
}

//Store/Load Accumulator directly
void Stlda(uint8_t loading_from_memory)
{
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;	

	address = (high_byte << 8) + low_byte;
	
	if(loading_from_memory)
	{
		a[0] = memory[address];
		return;
	}

	memory[address] = a[0];
}

//Store/Load register pair H directly
void Slhld(uint8_t loading_from_memory)
{
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;
	
	address = (high_byte << 8) + low_byte;
	
	if(loading_from_memory)
	{
		l[0] = memory[address + 0];
		h[0] = memory[address + 1];
		return;
	}

	memory[address + 0] = l[0];
	memory[address + 1] = h[0];
}

//Store/Load accumulator indirect
void Stldax(uint16_t *register_pair, uint8_t loading_from_memory)
{
	if(loading_from_memory)
	{
		a[0] = memory[register_pair[0]];
		return;
	}

	memory[register_pair[0]] = a[0];
}

//Exchange contents of register pair H with contents of register pair D
void Xchg()
{
	h[0] = h[0] ^ d[0];
	d[0] = d[0] ^ h[0];
	h[0] = h[0] ^ d[0];

	l[0] = l[0] ^ e[0];
	e[0] = e[0] ^ l[0];
	l[0] = l[0] ^ e[0];
}

//Arithmetic
//Subtract/Add contents of register to accumulator
void SubAddRegister(uint8_t *source_register, uint8_t adding)
{
	if(adding)
	{
		a[0] += source_register[0];
		return;
	}

	a[0] -= source_register[0];
}

//Subtract/Add contents of memory to accumulator
void SubAddMemory(uint8_t adding)
{
	if(adding)
	{
		a[0] += memory[h_pair[0]];
		return;
	}

	a[0] -= memory[h_pair[0]]; 
}

//Subtract/Add immediate
void SuiAdi(uint8_t adding)
{
	if(adding)
	{
		a[0] += memory[pc + 0];
		pc += 1;
		return;
	}

	a[0] -= memory[pc + 0];
	pc += 1;
}

//Subtract/Add register with borrow/carry
void SbbAdcRegister(uint8_t *source_register, uint8_t adding)
{
	if(adding)
	{
		a[0] += source_register[0] + (status[0] & 0x01);	//status bit 0 is the carry flag
		return; 
	}

	a[0] -= (source_register[0] + (status[0] & 0x01));
}

//Subtract/Add memory with borrow/carry
void SbbAdcMemory(uint8_t *addend_memory, uint8_t adding)
{
	if(adding)
	{
		a[0] += memory[h_pair[0]] + (status[0] & 0x01);		//status bit 0 is the carry flag
		return;
	}

	a[0] -= (memory[h_pair[0]] + (status[0] & 0x01)); 
}

//Subtract/Add immediate with carry
void SbiAci(uint8_t adding)
{
	if(adding)
	{
		a[0] += memory[pc + 0] + (status[0] & 0x01);		//status bit 0 is the carry flag
		pc += 1;
		return;
	}

	a[0] -= (memory[pc + 0] + (status[0] & 0x01));
	pc += 1;
}

//Decrement/Increment register
void DcrInrRegister(uint8_t *target_register, uint8_t incrementing)
{
	if(incrementing)
	{
		target_register[0] += 1;
		return;
	}

	target_register[0] -= 1;
}

//Decrement/Increment memory
void DcrInrMemory(uint8_t incrementing)
{
	if(incrementing)
	{
		memory[h_pair[0]] += 1; 
		return;
	}

	memory[h_pair[0]] -= 1;
}

//Decrement/Increment register pair
void DcxInx(uint16_t *register_pair, uint8_t incrementing)
{
	if(incrementing)
	{
		register_pair[0] += 1;
		return;
	}

	register_pair[0] -= 1;
}

//Add register pair to register pair H
void Dad(uint16_t *register_pair)
{
	h_pair[0] += register_pair[0];
}

//Decimal Adjust Accumulator
void Daa()
{
	if((a[0] & 0x0F) > 9 || (status[0] & 0x02))		//status bit 1 is the auxiliary carry flag
	{
		a[0] += 0x06;
	} 

	if(((a[0] & 0xF0) >> 4) > 9 || (status[0] & 0x01))	//status bit 0 is the carry flag
	{
		a[0] += 0x60;
	} 
}

//Logic
//AND register
void AnaRegister(uint8_t *source_register)
{
	a[0] &= source_register[0];
}

//AND memory
void AnaMemory()
{
	a[0] &= memory[h_pair[0]];
}

//AND immediate
void Ani()
{
	a[0] &= memory[pc + 0];
	pc += 1;
}

//XOR register
void XraRegister(uint8_t *source_register)
{
	a[0] ^= source_register[0];
}

//XOR memory
void XraMemory()
{
	a[0] ^= memory[h_pair[0]];
}

//XOR immediate
void Xri()
{
	a[0] ^= memory[pc + 0];
	pc += 1;
}

//OR register
void OraRegister(uint8_t *source_register)
{
	a[0] |= source_register[0];
}

//OR memory
void OraMemory()
{
	a[0] |= memory[h_pair[0]];
}

//OR immediate
void Ori()
{
	a[0] |= memory[pc + 0];
	pc += 1;
}

//Compare register
void CmpRegister(uint8_t *source_register)
{
	if(a[0] == source_register[0])
	{
		status[0] |= 0x08;	
	}

	if(a[0] < source_register[0])
	{
		status[0] |= 0x01;
	}
}

//Compare memory
void CmpMemory()
{
	if(a[0] == memory[h_pair[0]])
	{
		status[0] |= 0x08;	
	}

	if(a[0] < memory[h_pair[0]])
	{
		status[0] |= 0x01;
	}
}

//Compare immediate
void Cpi(data *input)
{
	if(a[0] == memory[pc + 0])
	{
		status[0] |= 0x08;	
	}

	if(a[0] < memory[pc + 0])
	{
		status[0] |= 0x01;
	}
	
	pc += 1;
}

//Rotate left

//Rotate right

//Rotate left through carry

//Rotate right through carry

//Complement accumulator

//Complement carry

//Set carry

//Branch

//Stack, IO, Machine Control


static instruction instruction_set[1] =
{	
	Cpi
};

//---

int main(int argv, char *argc[])
{
	time = 0;
	memory = (uint8_t*)malloc(MEMORY_SIZE * sizeof(uint8_t));
	
	while(1)
	{
		instruction_register = memory[pc];
		pc += 1;

		instruction_set[instruction_register]
			(&instruction_set_data[instruction_register]);
		 	
	}

	free(memory);			
	memory = NULL;

	return 0;
}

