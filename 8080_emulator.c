/************************************************************************
* 8080_Emulator								*
* Pramuka Perera							*
* June 23, 2017								*
* Emulator for the Intel 8080 processor as used in the Altair 8800 	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//HARDWARE---
//Memory
static uint8_t *memory;

//Registers 
static uint8_t register_file[10];

//General Purpose Registers
static uint8_t *const b = register_file + 0;
static uint8_t *const v = register_file + 1;
static uint8_t *const d = register_file + 2;
static uint8_t *const e = register_file + 3;
static uint8_t *const h = register_file + 4;
static uint8_t *const l = register_file + 5;

//Accumulator
static uint8_t *const a = register_file + 7;

//Status Byte
//FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
//BIT   - 0     | 1         | 2    | 3    | 4 
static uint8_t *const status = register_file + 6;
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

//use offset method (pc and sp contain emulated addresses, not addresses from host memory) so that if I read the value of the pc or sp, I get the emulated address and not the actual address of the host computer
static uint8_t pc;
static uint8_t sp;

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
	uint8_t size;
	uint8_t flags;
	uint8_t duration;
	//uint32_t (*instFunc)(void); //Function that emulates instruction; returns duration of instruction
	char *name;		//Instruction mnemonic		
} data;

//Function that emulates instruction
typedef void (*instruction)(data); 

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

static data instruction_set_data[246];

static instruction instruction_set[246];

//Instruction-Emulating Functions
//Data Transfer
//MOV REG -> Move data8 from source register to destination register
void MovRegister(uint8_t *destination_register, uint8_t *source_register)
{
	*destination_register = *source_register;	
}

//MOV MEM -> Move data8 from source register to destination memory (addr stored in register pair H) 
//	  OR Move data8 from source memory (addr stored in rp H) to destination register
//	  -> src_mem = 0 -> src = register, src_mem = 1 -> src = (H,L)
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

//MVI -> Moves immediate data8 to register or memory as selected by dest_mem (0 = register, 1 = memory)
void Mvi(uint8_t *destination, uint8_t destination_is_memory){
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

//LXI -> Loads data stored at location specified by immediate address (High Order Byte = byte 3, Low Order Byte = byte 2) to the specified register pair
void Lxi(uint8_t *register_pair){
	uint8_t high_byte, low_byte;
	uint16_t address;

	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;

	address = (high_byte << 8) + low_byte;
	register_pair[1] = memory[address + 0];
	register_pair[0] = memory[address + 1];
}

//LDA -> Loads data stored at location specified by immediate address to the accumulator register
//STA -> Stores data stored in accumulator to location specified by immediate address
//load_sel = 1, load data from addr to acc; load_sel = 0, store data from acc to addr
void Stlda(uint8_t loading){
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;	

	address = (high_byte << 8) + low_byte;
	
	if(loading)
	{
		a[0] = memory[address];
		return;
	}

	memory[address] = a[0];
}

//LHLD -> Loads data stored at location specified by immediate address to HL register pair
//SHLD -> Stores data in HL register pair to memory location specified by immediate address
//load_sel = 1, load data from addr to HL; load_sel = 0, store data from HL to addr
void Slhld(uint8_t loading){
	uint8_t high_byte, low_byte;
	uint16_t address;
	
	low_byte = memory[pc + 0];
	high_byte = memory[pc + 1];
	pc += 2;
	
	address = (high_byte << 8) + low_byte;
	
	if(loading)
	{
		l[0] = memory[address + 0];
		h[0] = memory[address + 1];
		return;
	}

	memory[address + 0] = l[0];
	memory[address + 1] = h[0];
}

void Stldax(uint16_t *register_pair, uint8_t loading){
	if(loading)
	{
		a[0] = memory[register_pair[0]];
		return;
	}

	memory[register_pair[0]] = a[0];
}

void Xchg(){
	h[0] = h[0] ^ d[0];
	d[0] = d[0] ^ h[0];
	h[0] = h[0] ^ d[0];

	l[0] = l[0] ^ e[0];
	e[0] = e[0] ^ l[0];
	l[0] = l[0] ^ e[0];
}

//Arithmetic

//Logic

//Branch

//Stack, IO, Machine Control

//---

void main(){
	memory = (uint8_t*)malloc(4000 * sizeof(uint8_t));

	while(1)
	{
		instruction_register = memory[pc];	
	}
	free(memory);			
}
