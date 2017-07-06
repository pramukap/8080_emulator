/************************************************************************
 * 8080_Emulator							*
 * Pramuka Perera							*
 * June 23, 2017							*
 * Emulator for the Intel 8080 processor as used in the Altair 8800 	*
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEMORY_SIZE		4000
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
static uint8_t *const c = register_file + 0;
static uint8_t *const b = register_file + 1;
static uint8_t *const e = register_file + 2;
static uint8_t *const d = register_file + 3;
static uint8_t *const l = register_file + 4;
static uint8_t *const h = register_file + 5;

//Accumulator
static uint8_t *const a = register_file + 6;

/*
 * Status Byte
 * FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
 * BIT   - 0     | 1         | 2    | 3    | 4 
 */
static uint8_t *const status = register_file + 7;

//Registers Z and W can only be used for instruction execution
//These registers are not directly accessible to the programmer
static uint8_t *const z = register_file + 8;
static uint8_t *const w = register_file + 9;

//Register Pairs
//B+C
static uint16_t *const b_pair = (uint16_t*)(register_file + 0);
//D+E
static uint16_t *const d_pair = (uint16_t*)(register_file + 2);
//H+L
static uint16_t *const h_pair = (uint16_t*)(register_file + 4);
//A+status
static uint16_t *const psw = (uint16_t*)(register_file + 6);

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
	//Bits			[15:12] [11:10]			  [9:5]         [4:0]
	//Data			        |SIZE(BYTES EXCL. OPCODE) |DURATION(CC) |FLAGS(Even Parity, Zero, Sign, Aux Carry, Carry)
	//uint16_t data;
	uint8_t *register_1;		//source register or only register mentioned in instruction
	uint8_t *register_2;		//destination register
	uint16_t *register_pair_1;	//source register pair or only register mentioned in instruction
	uint16_t *register_pair_2;	//destination register pair
	char name[12];			//Instruction mnemonic		
	uint8_t size;			//size of instruction in bytes
	uint8_t flags;			//flags triggered by instruction
	uint8_t duration;		//number of clock cycles instruction takes
} data; 

//Function that emulates instruction
typedef void (*instruction)(data *input); 

//-> size, flags, duration, function, name
//static data inst_data[246] = {		//	mnemonic      	SIZE,DURATION,FLAGS (EP, Z, S, AC, C)
/*
{0x0080,,"NOP"} 		//0	NOP 		S0,D4,F00000	0000 0000 1000 0000
{0x0940,,"LXI B, D16"}		//1	LXI B		S2,D10,F00000   0000 1001 0100 0000
{0x00E0,,"STAX B"}		//2	STAX B		S0,D7,F00000    0000 0000 1110 0000
{0x00A0,,"INX B"}		//3	INX B		S0,D5,F00000	0000 0000 1010 0000
{0x00BF,,"INR B"}		//4	INR B		S0,D5,F11110	0000 0000 1011 1111
{0x00BF,,"DCR B"}		//5	DCRB		S0,D5,F11110		"
{0x04E0,,"MVI B D8"}		//6	MVI B D8	S1,D7,F00000	0000 0100 1110 0000
{0x0081,,"RLC"}			//7	RLC		S0,D4,F00001	0000 0000 1000 0001
				//8
{0x0141,,"DAD B"}		//9	DAD B		S0,D10,F00001	0000 0001 0100 0001
{0x00E0,,"LDAX B"}		//10	LDAX B		S0,D7,F00000	0000 0000 1110 0000
{0x00A0,,"DCX B"}		//11	DCX B		S0,D5,F00000	0000 0000 1010 0000
{0x00DE,,"INR C"}		//12	INR C		S0,D5,F11110	0000 0000 1011 1110
*/
/*
{0x00000, }
};

static inst inst_set[246] = {

}
*/

const uint16_t place_holder = 0xFFFF;
const uint8_t const *ph8_ptr = &((uint8_t)place_holder);
const uint16_t const *ph16_ptr = &place_holder;

/*
 * Status Byte
 * FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
 * BIT   - 0     | 1         | 2    | 3    | 4 
 */
//r1 | r2 | rp1 | rp2 | name | size | flags | duration 
static const data instruction_set_data[256] = 
{
	/*00*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
		{ph8_ptr, ph8_ptr, b_pair, ph16_ptr, "LXI B, D16", 3, NONE, },
	/*02*/	{ph8_ptr, ph8_ptr, b_pair, ph16_ptr, "STAX B", 1, NONE, },
		{ph8_ptr, ph8_ptr, b_pair, ph16_ptr, "INX B", 1, NONE, },
	/*04*/	{b, ph8_ptr, ph16_ptr, ph16_ptr, "INR B", 1, ALL_EXCEPT_CARRY, },
		{b, ph8_ptr, ph16_ptr, ph16_ptr, "DCR B", 1, ALL_EXCEPT_CARRY, },
	/*06*/ 	{b, ph8_ptr, ph16_ptr, ph16_ptr, "MVI B, D8", 2, NONE, },
	 	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "RLC", 1, CARRY, },
	/*08*/ 	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
	 	{ph8_ptr, ph8_ptr, b_pair, ph16_ptr, "DAD B", 1, CARRY, },
	/*0A*/ 	{ph8_ptr, ph8_ptr, b_pair, ph16_ptr, "LDAX B", 1, NONE, },
	 	{ph8_ptr, ph8_ptr, b_pair, ph16_ptr, "DCX B", 1, NONE, },
	/*0C*/ 	{c, ph8_ptr, ph16_ptr, ph16_ptr, "INR C", 1, ALL_EXCEPT_CARRY, },
	 	{c, ph8_ptr, ph16_ptr, ph16_ptr, "DCR C", 1, ALL_EXCEPT_CARRY, },
	/*0E*/	{c, ph8_ptr, ph16_ptr, ph16_ptr, "MVI C, D8", 2, NONE, },
		{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "RRC", 1, CARRY, },
	/*10*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
		{ph8_ptr, ph8_ptr, d_pair, ph16_ptr, "LXI D, D16", 3, NONE, },
	/*12*/	{ph8_ptr, ph8_ptr, d_pair, ph16_ptr, "STAX D", 1, NONE, },
		{ph8_ptr, ph8_ptr, d_pair, ph16_ptr, "INX D", 1, NONE, },
	/*14*/	{d, ph8_ptr, ph16_ptr, ph16_ptr, "INR D", 1, ALL_EXCEPT_CARRY, },
		{d, ph8_ptr, ph16_ptr, ph16_ptr, "DCR D", 1, ALL_EXCEPT_CARRY, },
	/*16*/	{d, ph8_ptr, ph16_ptr, ph16_ptr, "MVI D, D8", 2, NONE, },
		{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "RAL", 1, CARRY, },
	/*18*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
		{ph8_ptr, ph8_ptr, d_pair, ph16_ptr, "DAD D", 1, CARRY, },
	/*1A*/	{ph8_ptr, ph8_ptr, d_pair, ph16_ptr, "LDAX D", 1, NONE, },
		{ph8_ptr, ph8_ptr, d_pair, ph16_ptr, "DCX D", 1, NONE, },
	/*1C*/	{e, ph8_ptr, ph16_ptr, ph16_ptr, "INR E", 1, ALL_EXCEPT_CARRY, },
		{e, ph8_ptr, ph16_ptr, ph16_ptr, "DCR E", 1, ALL_EXCEPT_CARRY, },
	/*1E*/	{e, ph8_ptr, ph16_ptr, ph16_ptr, "MVI E, D8", 2, NONE, },
		{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "RAR", 1, CARRY, },
	/*20*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
		{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "LXI H, D16", 3, NONE, },
	/*22*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "SHLD ADR", 3, NONE, },
		{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "INX H", 1, NONE, },
	/*24*/	{h, ph8_ptr, ph16_ptr, ph16_ptr, "INR H", 1, ALL_EXCEPT_CARRY, },
		{h, ph8_ptr, ph16_ptr, ph16_ptr, "DCR H", 1, ALL_EXCEPT_CARRY, },
	/*26*/	{h, ph8_ptr, ph16_ptr, ph16_ptr, "MVI H, D8", 2, NONE, },
		{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "DAA", 1, NONE, },
	/*28*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
		{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "DAD H", 1, CARRY, },
	/*2A*/	{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "LHLD ADR", 3, NONE, },
		{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "DCX H", 1, NONE, },
	/*2C*/	{l, ph8_ptr, ph16_ptr, ph16_ptr, "INR L", 1, ALL_EXCEPT_CARRY, },
		{l, ph8_ptr, ph16_ptr, ph16_ptr, "DCR L", 1, ALL_EXCEPT_CARRY, },
	/*2E*/	{l, ph8_ptr, ph16_ptr, ph16_ptr, "MVI L, D8", 2, NONE, },
		{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "CMA", 1, NONE, },
	/*30*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
		{ph8_ptr, ph8_ptr, &sp, ph16_ptr, "LXI SP, D16", 3, NONE, },
	/*32*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "STA ADR", 3, NONE, },
		{ph8_ptr, ph8_ptr, &sp, ph16_ptr, "INX SP", 1, NONE, },
	/*34*/	{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "INR M", 1, ALL_EXCEPT_CARRY, },
		{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "DCR M", 1, ALL_EXCEPT_CARRY, },
	/*36*/	{ph8_ptr, ph8_ptr, h_pair, ph16_ptr, "MVI M, D8", 2, NONE, },
		{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "STC", 1, CARRY, },
	/*38*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "NOP", 1, NONE, },
		{ph8_ptr, ph8_ptr, &sp, ph16_ptr, "DAD SP", 1, CARRY, },
	/*3A*/	{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "LDA adr", 3, NONE, },
		{ph8_ptr, ph8_ptr, &sp, ph16_ptr, "DCX SP", 1, NONE, },
	/*3C*/	{a, ph8_ptr, ph16_ptr, ph16_ptr, "INR A", 1, ALL_EXCEPT_CARRY, },
		{a, ph8_ptr, ph16_ptr, ph16_ptr, "DCR A", 1, ALL_EXCEPT_CARRY, },
	/*3E*/	{a, ph8_ptr, ph16_ptr, ph16_ptr, "MVI A, D8", 2, NONE, },
		{ph8_ptr, ph8_ptr, ph16_ptr, ph16_ptr, "CMC", 1, CARRY, },
	/*40*/	{b, b, ph16_ptr, ph16_ptr, "MOV B, B", 1, NONE, },
		{c, b, ph16_ptr, ph16_ptr, "MOV B, C", 1, NONE, },
	/*42*/	{d, b, ph16_ptr, ph16_ptr, "MOV B, D", 1, NONE, },
		{e, b, ph16_ptr, ph16_ptr, "MOV B, E", 1, NONE, },
	/*44*/	{h, b, ph16_ptr, ph16_ptr, "MOV B, H", 1, NONE, },
		{l, b, ph16_ptr, ph16_ptr, "MOV B, L", 1, NONE, },
	/*46*/	{ph8_ptr, b, ph16_ptr, ph16_ptr, "MOV B, M", 1, NONE, },
		{a, b, ph16_ptr, ph16_ptr, "MOV B, A", 1, NONE, },
	/*48*/	{b, c, ph16_ptr, ph16_ptr, "MOV C, B", 1, NONE, },
		{c, c, ph16_ptr, ph16_ptr, "MOV C, C", 1, NONE, },
	/*4A*/	{d, c, ph16_ptr, ph16_ptr, "MOV C, D", 1, NONE, },
		{e, c, ph16_ptr, ph16_ptr, "MOV C, E", 1, NONE, },
	/*4C*/	{h, c, ph16_ptr, ph16_ptr, "MOV C, H", 1, NONE, },
		{l, c, ph16_ptr, ph16_ptr, "MOV C, L", 1, NONE, },
	/*4E*/	{ph8_ptr, c, ph16_ptr, ph16_ptr, "MOV C, M", 1, NONE, },
		{a, c, ph16_ptr, ph16_ptr, "MOV C, A", 1, NONE, },
	/*50*/	{b, d, ph16_ptr, ph16_ptr, "MOV D, B", 1, NONE, },
		{c, d, ph16_ptr, ph16_ptr, "MOV D, C", 1, NONE, },
	/*52*/	{d, d, ph16_ptr, ph16_ptr, "MOV D, D", 1, NONE, },
		{e, d, ph16_ptr, ph16_ptr, "MOV D, E", 1, NONE, },
	/*54*/	{h, d, ph16_ptr, ph16_ptr, "MOV D, H", 1, NONE, },
		{l, d, ph16_ptr, ph16_ptr, "MOV D, L", 1, NONE, },
	/*56*/	{ph8_ptr, d, ph16_ptr, ph16_ptr, "MOV D, M", 1, NONE, },
		{a, d, ph16_ptr, ph16_ptr, "MOV D, A", 1, NONE, },
	/*58*/	{b, e, ph16_ptr, ph16_ptr, "MOV E, B", 1, NONE, },
		{c, e, ph16_ptr, ph16_ptr, "MOV E, C", 1, NONE, },
	/*5A*/	{d, e, ph16_ptr, ph16_ptr, "MOV E, D", 1, NONE, },
		{e, e, ph16_ptr, ph16_ptr, "MOV E, E", 1, NONE, },
	/*5C*/	{h, e, ph16_ptr, ph16_ptr, "MOV E, H", 1, NONE, },
		{l, e, ph16_ptr, ph16_ptr, "MOV E, L", 1, NONE, },
	/*5E*/	{ph8_ptr, e, ph16_ptr, ph16_ptr, "MOV E, M", 1, NONE, },
		{a, e, ph16_ptr, ph16_ptr, "MOV E, A", 1, NONE, },
	/*60*/	{b, h, ph16_ptr, ph16_ptr, "MOV H, B", 1, NONE, },
		{c, h, ph16_ptr, ph16_ptr, "MOV H, C", 1, NONE, },
	/*62*/	{d, h, ph16_ptr, ph16_ptr, "MOV H, D", 1, NONE, },
		{e, h, ph16_ptr, ph16_ptr, "MOV H, E", 1, NONE, },
	/*64*/	{h, h, ph16_ptr, ph16_ptr, "MOV H, H", 1, NONE, },
		{l, h, ph16_ptr, ph16_ptr, "MOV H, L", 1, NONE, },
	/*66*/	{ph8_ptr, h, ph16_ptr, ph16_ptr, "MOV H, M", 1, NONE, },
		{a, h, ph16_ptr, ph16_ptr, "MOV H, A", 1, NONE, },
	/*68*/	
};

static instruction instruction_set[246];

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
void Cpi()
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

//---

int main(int argv, char *argc[])
{
	memory = (uint8_t*)malloc(MEMORY_SIZE * sizeof(uint8_t));

	while(1)
	{
		instruction_register = memory[pc];
		pc += 1;	
	}

	free(memory);			

	return 0;
}

