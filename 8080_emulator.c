/************************************************************************
 * 8080_Emulator							*
 * Pramuka Perera							*
 * June 23, 2017							*
 * Emulator for the Intel 8080 processor as used in the Altair 8800 	*
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NULL8_PTR		((void *)0)
#define NULL16_PTR		((uint16_t *)0)

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
static uint32_t time = 0;
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
	uint16_t *const register_pair_1;	//source register pair or only register mentioned in instruction
	const char name[12];			//Instruction mnemonic		
	const uint8_t size;			//size of instruction in bytes
	const uint8_t flags;			//flags triggered by instruction
	const uint8_t duration;			//number of clock cycles instruction takes (instructions marked that have 0xFF have (11 or 17) cc or (5 or 11) cc
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
	/*00*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4 },
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "LXI B, D16", 3, NONE, 10},
	/*02*/	{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "STAX B", 1, NONE, 7},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "INX B", 1, NONE, 5},
	/*04*/	{register_file + B, NULL8_PTR, NULL16_PTR, "INR B", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + B, NULL8_PTR, NULL16_PTR, "DCR B", 1, ALL_EXCEPT_CARRY, 5},
	/*06*/ 	{register_file + B, NULL8_PTR, NULL16_PTR, "MVI B, D8", 2, NONE, 7},
	 	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RLC", 1, CARRY, 4},
	/*08*/ 	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
	 	{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "DAD B", 1, CARRY, 10},
	/*0A*/ 	{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "LDAX B", 1, NONE, 7},
	 	{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "DCX B", 1, NONE, 5},
	/*0C*/ 	{register_file + C, NULL8_PTR, NULL16_PTR, "INR C", 1, ALL_EXCEPT_CARRY, 5},
	 	{register_file + C, NULL8_PTR, NULL16_PTR, "DCR C", 1, ALL_EXCEPT_CARRY, 5},
	/*0E*/	{register_file + C, NULL8_PTR, NULL16_PTR, "MVI C, D8", 2, NONE, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RRC", 1, CARRY, 4},
	/*10*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "LXI D, D16", 3, NONE, 10},
	/*12*/	{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "STAX D", 1, NONE, 7},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "INX D", 1, NONE, 5},
	/*14*/	{register_file + D, NULL8_PTR, NULL16_PTR, "INR D", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + D, NULL8_PTR, NULL16_PTR, "DCR D", 1, ALL_EXCEPT_CARRY, 5},
	/*16*/	{register_file + D, NULL8_PTR, NULL16_PTR, "MVI D, D8", 2, NONE, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RAL", 1, CARRY, 4},
	/*18*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "DAD D", 1, CARRY, 10},
	/*1A*/	{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "LDAX D", 1, NONE, 7},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "DCX D", 1, NONE, 5},
	/*1C*/	{register_file + E, NULL8_PTR, NULL8_PTR, "INR E", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + E, NULL8_PTR, NULL8_PTR, "DCR E", 1, ALL_EXCEPT_CARRY, 5},
	/*1E*/	{register_file + E, NULL8_PTR, NULL8_PTR, "MVI E, D8", 2, NONE, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RAR", 1, CARRY, 4},
	/*20*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + H_PAIR, "LXI H, D16", 3, NONE, 10},
	/*22*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "SHLD ADR", 3, NONE, 16},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + H_PAIR, "INX H", 1, NONE, 5},
	/*24*/	{register_file + H, NULL8_PTR, NULL16_PTR, "INR H", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + H, NULL8_PTR, NULL16_PTR, "DCR H", 1, ALL_EXCEPT_CARRY, 5},
	/*26*/	{register_file + H, NULL8_PTR, NULL16_PTR, "MVI H, D8", 2, NONE, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "DAA", 1, NONE, 4},
	/*28*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + H_PAIR, "DAD H", 1, CARRY, 10},
	/*2A*/	{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + H_PAIR, "LHLD ADR", 3, NONE, 16},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + H_PAIR, "DCX H", 1, NONE, 5},
	/*2C*/	{register_file + L, NULL8_PTR, NULL16_PTR, "INR L", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + L, NULL8_PTR, NULL16_PTR, "DCR L", 1, ALL_EXCEPT_CARRY, 5},
	/*2E*/	{register_file + L, NULL8_PTR, NULL16_PTR, "MVI L, D8", 2, NONE, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CMA", 1, NONE, 4},
	/*30*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
		{NULL8_PTR, NULL8_PTR, &sp, "LXI SP, D16", 3, NONE, 10},
	/*32*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "STA ADR", 3, NONE, 13},
		{NULL8_PTR, NULL8_PTR, &sp, "INX SP", 1, NONE, 5},
	/*34*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "INR M", 1, ALL_EXCEPT_CARRY, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "DCR M", 1, ALL_EXCEPT_CARRY, 10},
	/*36*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "MVI M, D8", 2, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "STC", 1, CARRY, 4},
	/*38*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
		{NULL8_PTR, NULL8_PTR, &sp, "DAD SP", 1, CARRY, 10},
	/*3A*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "LDA adr", 3, NONE, 13},
		{NULL8_PTR, NULL8_PTR, &sp, "DCX SP", 1, NONE, 5},
	/*3C*/	{register_file + A, NULL8_PTR, NULL16_PTR, "INR A", 1, ALL_EXCEPT_CARRY, 5},
		{register_file + A, NULL8_PTR, NULL16_PTR, "DCR A", 1, ALL_EXCEPT_CARRY, 5},
	/*3E*/	{register_file + A, NULL8_PTR, NULL16_PTR, "MVI A, D8", 2, NONE, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CMC", 1, CARRY, 4},
	/*40*/	{register_file + B, register_file + B, NULL16_PTR, "MOV B, B", 1, NONE, 5},
		{register_file + C, register_file + B, NULL16_PTR, "MOV B, C", 1, NONE, 5},
	/*42*/	{register_file + D, register_file + B, NULL16_PTR, "MOV B, D", 1, NONE, 5},
		{register_file + E, register_file + B, NULL16_PTR, "MOV B, E", 1, NONE, 5},
	/*44*/	{register_file + H, register_file + B, NULL16_PTR, "MOV B, H", 1, NONE, 5},
		{register_file + L, register_file + B, NULL16_PTR, "MOV B, L", 1, NONE, 5},
	/*46*/	{NULL8_PTR, register_file + B, NULL16_PTR, "MOV B, M", 1, NONE, 7},
		{register_file + A, register_file + B, NULL16_PTR, "MOV B, A", 1, NONE, 5},
	/*48*/	{register_file + B, register_file + C, NULL16_PTR, "MOV C, B", 1, NONE, 5},
		{register_file + C, register_file + C, NULL16_PTR, "MOV C, C", 1, NONE, 5},
	/*4A*/	{register_file + D, register_file + C, NULL16_PTR, "MOV C, D", 1, NONE, 5},
		{register_file + E, register_file + C, NULL16_PTR, "MOV C, E", 1, NONE, 5},
	/*4C*/	{register_file + H, register_file + C, NULL16_PTR, "MOV C, H", 1, NONE, 5},
		{register_file + L, register_file + C, NULL16_PTR, "MOV C, L", 1, NONE, 5},
	/*4E*/	{NULL8_PTR, register_file + C, NULL16_PTR, "MOV C, M", 1, NONE, 7},
		{register_file + A, register_file + C, NULL16_PTR, "MOV C, A", 1, NONE, 5},
	/*50*/	{register_file + B, register_file + D, NULL16_PTR, "MOV D, B", 1, NONE, 5},
		{register_file + C, register_file + D, NULL16_PTR, "MOV D, C", 1, NONE, 5},
	/*52*/	{register_file + D, register_file + D, NULL16_PTR, "MOV D, D", 1, NONE, 5},
		{register_file + E, register_file + D, NULL16_PTR, "MOV D, E", 1, NONE, 5},
	/*54*/	{register_file + H, register_file + D, NULL16_PTR, "MOV D, H", 1, NONE, 5},
		{register_file + L, register_file + D, NULL16_PTR, "MOV D, L", 1, NONE, 5},
	/*56*/	{NULL8_PTR, register_file + D, NULL16_PTR, "MOV D, M", 1, NONE, 7},
		{register_file + A, register_file + D, NULL16_PTR, "MOV D, A", 1, NONE, 5},
	/*58*/	{register_file + B, register_file + E, NULL16_PTR, "MOV E, B", 1, NONE, 5},
		{register_file + C, register_file + E, NULL16_PTR, "MOV E, C", 1, NONE, 5},
	/*5A*/	{register_file + D, register_file + E, NULL16_PTR, "MOV E, D", 1, NONE, 5},
		{register_file + E, register_file + E, NULL16_PTR, "MOV E, E", 1, NONE, 5},
	/*5C*/	{register_file + H, register_file + E, NULL16_PTR, "MOV E, H", 1, NONE, 5},
		{register_file + L, register_file + E, NULL16_PTR, "MOV E, L", 1, NONE, 5},
	/*5E*/	{NULL8_PTR, register_file + E, NULL16_PTR, "MOV E, M", 1, NONE, 7},
		{register_file + A, register_file + E, NULL16_PTR, "MOV E, A", 1, NONE, 5},
	/*60*/	{register_file + B, register_file + H, NULL16_PTR, "MOV H, B", 1, NONE, 5},
		{register_file + C, register_file + H, NULL16_PTR, "MOV H, C", 1, NONE, 5},
	/*62*/	{register_file + D, register_file + H, NULL16_PTR, "MOV H, D", 1, NONE, 5},
		{register_file + E, register_file + H, NULL16_PTR, "MOV H, E", 1, NONE, 5},
	/*64*/	{register_file + H, register_file + H, NULL16_PTR, "MOV H, H", 1, NONE, 5},
		{register_file + L, register_file + H, NULL16_PTR, "MOV H, L", 1, NONE, 5},
	/*66*/	{NULL8_PTR, register_file + H, NULL16_PTR, "MOV H, M", 1, NONE, 7},
		{register_file + A, register_file + H, NULL16_PTR, "MOV H, A", 1, NONE, 5},
	/*68*/	{register_file + B, register_file + L, NULL16_PTR, "MOV L, B", 1, NONE, 5},
		{register_file + C, register_file + L, NULL16_PTR, "MOV L, C", 1, NONE, 5},
	/*6A*/	{register_file + D, register_file + L, NULL16_PTR, "MOV L, D", 1, NONE, 5},
		{register_file + E, register_file + L, NULL16_PTR, "MOV L, E", 1, NONE, 5},
	/*6C*/	{register_file + H, register_file + L, NULL16_PTR, "MOV L, H", 1, NONE, 5},
		{register_file + L, register_file + L, NULL16_PTR, "MOV L, L", 1, NONE, 5},
	/*6E*/	{NULL8_PTR, register_file + L, NULL16_PTR, "MOV L, M", 1, NONE, 7},
		{register_file + A, register_file + L, NULL16_PTR, "MOV L, A", 1, NONE, 5},
	/*70*/	{register_file + B, NULL8_PTR, NULL16_PTR, "MOV M, B", 1, NONE, 7}, 
		{register_file + C, NULL8_PTR, NULL16_PTR, "MOV M, C", 1, NONE, 7}, 
	/*72*/	{register_file + D, NULL8_PTR, NULL16_PTR, "MOV M, D", 1, NONE, 7}, 
		{register_file + E, NULL8_PTR, NULL16_PTR, "MOV M, E", 1, NONE, 7}, 
	/*74*/	{register_file + H, NULL8_PTR, NULL16_PTR, "MOV M, H", 1, NONE, 7}, 
		{register_file + L, NULL8_PTR, NULL16_PTR, "MOV M, L", 1, NONE, 7}, 
	/*76*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "HLT", 1, NONE, 7}, 
		{register_file + A, NULL8_PTR, NULL16_PTR, "MOV M, A", 1, NONE, 7},
	/*78*/	{register_file + B, register_file + A, NULL16_PTR, "MOV A, B", 1, NONE, 5},
		{register_file + C, register_file + A, NULL16_PTR, "MOV A, C", 1, NONE, 5},
	/*7A*/	{register_file + D, register_file + A, NULL16_PTR, "MOV A, D", 1, NONE, 5},
		{register_file + E, register_file + A, NULL16_PTR, "MOV A, E", 1, NONE, 5},
	/*7C*/	{register_file + H, register_file + A, NULL16_PTR, "MOV A, H", 1, NONE, 5},
		{register_file + L, register_file + A, NULL16_PTR, "MOV A, L", 1, NONE, 5},
	/*7E*/	{NULL8_PTR, register_file + A, NULL16_PTR, "MOV A, M", 1, NONE, 7},
		{register_file + A, register_file + A, NULL16_PTR, "MOV A, A", 1, NONE, 5},
 	/*80*/	{register_file + B, NULL8_PTR, NULL16_PTR, "ADD B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "ADD C", 1, ALL, 4},
	/*82*/	{register_file + D, NULL8_PTR, NULL16_PTR, "ADD D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "ADD E", 1, ALL, 4},
	/*84*/	{register_file + H, NULL8_PTR, NULL16_PTR, "ADD H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "ADD L", 1, ALL, 4},
	/*86*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ADD M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "ADD A", 1, ALL, 4},
	/*88*/	{register_file + B, NULL8_PTR, NULL16_PTR, "ADC B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "ADC C", 1, ALL, 4},
	/*8A*/	{register_file + D, NULL8_PTR, NULL16_PTR, "ADC D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "ADC E", 1, ALL, 4},
	/*8C*/	{register_file + H, NULL8_PTR, NULL16_PTR, "ADC H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "ADC L", 1, ALL, 4},
	/*8E*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ADC M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "ADC A", 1, ALL, 4},
	/*90*/	{register_file + B, NULL8_PTR, NULL16_PTR, "SUB B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "SUB C", 1, ALL, 4},
	/*92*/	{register_file + D, NULL8_PTR, NULL16_PTR, "SUB D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "SUB E", 1, ALL, 4},
	/*94*/	{register_file + H, NULL8_PTR, NULL16_PTR, "SUB H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "SUB L", 1, ALL, 4},
	/*96*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "SUB M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "SUB A", 1, ALL, 4},
	/*98*/	{register_file + B, NULL8_PTR, NULL16_PTR, "SBB B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "SBB C", 1, ALL, 4},
	/*9A*/	{register_file + D, NULL8_PTR, NULL16_PTR, "SBB D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "SBB E", 1, ALL, 4},
	/*9C*/	{register_file + H, NULL8_PTR, NULL16_PTR, "SBB H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "SBB L", 1, ALL, 4},
	/*9E*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "SBB M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "SBB A", 1, ALL, 4},
	/*A0*/	{register_file + B, NULL8_PTR, NULL16_PTR, "ANA B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "ANA C", 1, ALL, 4},
	/*A2*/	{register_file + D, NULL8_PTR, NULL16_PTR, "ANA D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "ANA E", 1, ALL, 4},
	/*A4*/	{register_file + H, NULL8_PTR, NULL16_PTR, "ANA H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "ANA L", 1, ALL, 4},
	/*A6*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ANA M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "ANA A", 1, ALL, 4},
	/*A8*/	{register_file + B, NULL8_PTR, NULL16_PTR, "XRA B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "XRA C", 1, ALL, 4},
	/*AA*/	{register_file + D, NULL8_PTR, NULL16_PTR, "XRA D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "XRA E", 1, ALL, 4},
	/*AC*/	{register_file + H, NULL8_PTR, NULL16_PTR, "XRA H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "XRA L", 1, ALL, 4},
	/*AE*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "XRA M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "XRA A", 1, ALL, 4},
	/*B0*/	{register_file + B, NULL8_PTR, NULL16_PTR, "ORA B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "ORA C", 1, ALL, 4},
	/*B2*/	{register_file + D, NULL8_PTR, NULL16_PTR, "ORA D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "ORA E", 1, ALL, 4},
	/*B4*/	{register_file + H, NULL8_PTR, NULL16_PTR, "ORA H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "ORA L", 1, ALL, 4},
	/*B6*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ORA M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "ORA A", 1, ALL, 4},
	/*B8*/	{register_file + B, NULL8_PTR, NULL16_PTR, "CMP B", 1, ALL, 4},
		{register_file + C, NULL8_PTR, NULL16_PTR, "CMP C", 1, ALL, 4},
	/*BA*/	{register_file + D, NULL8_PTR, NULL16_PTR, "CMP D", 1, ALL, 4},
		{register_file + E, NULL8_PTR, NULL16_PTR, "CMP E", 1, ALL, 4},
	/*BC*/	{register_file + H, NULL8_PTR, NULL16_PTR, "CMP H", 1, ALL, 4},
		{register_file + L, NULL8_PTR, NULL16_PTR, "CMP L", 1, ALL, 4},
	/*BE*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CMP M", 1, ALL, 7},
		{register_file + A, NULL8_PTR, NULL16_PTR, "CMP A", 1, ALL, 4},
	/*C0*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RNZ", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "POP B", 1, NONE, 10},
	/*C2*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JNZ ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JMP ADR", 3, NONE, 10},
	/*C4*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CNZ ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + B_PAIR, "PUSH B", 1, NONE, 11},
	/*C6*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ADI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 0", 1, NONE, 11},
	/*C8*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RZ", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RET", 1, NONE, 10},
	/*CA*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JZ ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
	/*CC*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CZ ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CALL ADR", 3, NONE, 17},
	/*CE*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ACI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 1", 1, NONE, 11},
	/*D0*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RNC", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "POP D", 1, NONE, 10},
	/*D2*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JNC ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "OUT D8", 2, NONE, 10},
	/*D4*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CNC ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + D_PAIR, "PUSH D", 1, NONE, 11},
	/*D6*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "SUI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 2", 1, NONE, 11},
	/*D8*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RC", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
	/*DA*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JC ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "IN D8", 2, NONE, 10},
	/*DC*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CC ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
	/*DE*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "SBI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 3", 1, NONE, 11},
	/*E0*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RPO", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + H_PAIR, "POP H", 1, NONE, 10},
	/*E2*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JPO ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "XTHL", 1, NONE, 18},
	/*E4*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CPO ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + H_PAIR, "PUSH H", 1, NONE, 11},
	/*E6*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ANI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 4", 1, NONE, 11},
	/*E8*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RPE", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "PCHL", 1, NONE, 5},
	/*EA*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JPE ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "XCHG", 1, NONE, 4},
	/*EC*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CPE ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
	/*EE*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "XRI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 5", 1, NONE, 11},
	/*F0*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RP", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + PSW, "POP PSW", 1, NONE, 10},
	/*F2*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JP ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "DI", 1, NONE, 4},
	/*F4*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CP ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, ((uint16_t *)register_file) + PSW, "PUSH PSW", 1, NONE, 11},
	/*F6*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "ORI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 6", 1, NONE, 11},
	/*F8*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RM", 1, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "SPHL", 1, NONE, 5},
	/*FA*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "JM ADR", 3, NONE, 10},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "EI", 1, NONE, 4},
	/*FC*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CM ADR", 3, NONE, 0xFF},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "NOP", 1, NONE, 4},
	/*FE*/	{NULL8_PTR, NULL8_PTR, NULL16_PTR, "CPI D8", 2, ALL, 7},
		{NULL8_PTR, NULL8_PTR, NULL16_PTR, "RST 7", 1, NONE, 11}	
};

//Instruction-Emulating Functions
//Data Transfer
//Move contents of source register to destination register
void MovRegister(uint8_t *destination_register, uint8_t *source_register, uint8_t select, uint16_t data)
{
	*destination_register = *source_register;	
}

//Move to memory
void MovMemory(uint8_t *destination, uint8_t *source, uint8_t source_is_memory)
{
	uint16_t address;		

	if(source_is_memory)
	{
		address = (source[0] << 8) + source[1];
		
		destination[0] = memory[address];
		
		return;
	}

	address = (destination[0] << 8) + destination[1];
	
	memory[address] = source[0];
}

//Move to memory immediate
void Mvi(uint8_t *destination, uint8_t destination_is_memory)
{
	uint16_t address;	

	if(destination_is_memory)
	{
		address = (destination[0] << 8) + destination[1];
		
		memory[address] = memory[pc];
		pc += 1;
		
		return;	
	}

	destination[0] = memory[pc];
	pc += 1;
}

//Load register pair immediate
//void Lxi(uint8_t *register_pair)
void Lxi(data *input)
{
	uint8_t high_byte, low_byte;
	uint16_t address;

	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;

	address = (high_byte << 8) + low_byte;
	
	input->register_pair_1[1] = memory[address + 0];
	input->register_pair_1[0] = memory[address + 1];
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
	memory = (uint8_t*)malloc(MEMORY_SIZE * sizeof(uint8_t));
	instruction s = instruction_set[0];
	while(1)
	{
		instruction_register = memory[pc];
		pc += 1;

		instruction_set[instruction_register]
			(&instruction_set_data[instruction_register]);
		 	
	}

	free(memory);			

	return 0;
}

