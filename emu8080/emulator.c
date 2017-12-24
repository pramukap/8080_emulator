/************************************************************************
 * 8080_Emulator v0.0.0							*
 * Pramuka Perera							*
 * June 23, 2017							*
 * Emulator for the Intel 8080 processor			 	*
 * Operating at 2.5 MHz							*
 * TODO:								*
 *	Finish Display							*
 *	Test instructions						*
 *	Refactor							*
 *	Improve get program						*
 ************************************************************************/

#ifndef INCLUDE
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

#include "instruction_set.h"
#include "storage.h"
#include "vt100.h"

//HARDWARE---
uint8_t	*address_space,
	*hard_disk,
	*memory,
	*video_memory,
	*io;

uint8_t register_file[10];

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
uint8_t *const c = register_file + C;
uint8_t *const b = register_file + B;
uint8_t *const e = register_file + E;
uint8_t *const d = register_file + D;
uint8_t *const l = register_file + L;
uint8_t *const h = register_file + H;

//Accumulator
uint8_t *const a = register_file + A;

/*
 * Status Byte
 * FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
 * BIT   - 0     | 1         | 2    | 3    | 4 
 */
uint8_t *const status = register_file + STATUS;

//Registers Z and W can only be used for instruction execution
//These registers are not directly accessible to the programmer
//static uint8_t *const z = register_file + Z;
//static uint8_t *const w = register_file + W;

//Register Pairs
//B+C
uint16_t *const b_pair = (uint16_t*)(register_file + B_PAIR);
//D+E
uint16_t *const d_pair = (uint16_t*)(register_file + D_PAIR);
//H+L
uint16_t *const h_pair = (uint16_t*)(register_file + H_PAIR);
//A+status
uint16_t *const psw = (uint16_t*)(register_file + PSW);

/*
 * The pc and sp contain the emulated address and not the host address.
 * The emulated address is in fact an index for the emulated memory,
 * which is a dynamically-allocated array.
 */
uint16_t pc;
uint16_t sp;

//Instruction Register
uint8_t instruction_register;

//Processor Time
uint32_t time;
//---

/*
 * I/O---
 * Control I/O Signals of 8080
 * SIGNALS - WR'(0) | DBIN(O) | INTE(O) | INT(I) | HOLD ACK(O) | HOLD(I) | WAIT (0) | READY(I) | SYNC(O) | RESET(I) 
 * BIT     - 0      | 1       | 2       | 3      | 4           | 5       | 6        | 7        | 8       | 9
 */
uint16_t control;
uint8_t interrupt_enable;
uint8_t halt_enable;
uint8_t interrupt_request;
uint8_t priority;
//---

/*
 * USER INTERFACE---
 * Signals presented on the 8800 front panel
 * SIGNALS - INTE | PROT | MEMR | INP | M1 | OUT | HLTA | STACK | WO'  | INT(A) | WAIT | HLDA | RESET
 * BIT     - 0    | 1    | 2    | 3   | 4  | 5   | 6    | 7     | 8    | 9      | 10   | 11   | 12
 */
uint16_t indicator;
//---
//---

/*
 * Memory-mapped Nonvolatile Memory Registers
 * 0x3ffc - Storage Control Register
 * BITS:	3			2			1		0		
 * VALUE:	Write-Request Flag	Read-Request Flag	Ready Flag	Interrupt-Enable
 * 0x3ffd - Data Register
 * 0x3ffe/f - Address Registers 
 */

/* Memory-mapped Keyboard Registers
 * 0x3ff9 - Keyboard Control Register
 * BITS:	1		0
 * VALUE:	Ready Flag	Interrupt-Enable
 * 0x3ffa - Data Register
 */

/* Memory-mapped Display Registers
 *
 */ 

interrupt_device interrupt_vector;

void InterruptCheckAndInstructionFetch()
{
	if (interrupt_request && interrupt_enable)
	{
		interrupt_request = 0;

		switch (interrupt_vector){
		case STORAGE_READ:
			instruction_register = 0xd7;	//RST 02
			return;
		case STORAGE_WRITE:
			instruction_register = 0xdf;	//RST 03
			return;
		case KEYBOARD:
			instruction_register = 0xe7;	//RST 04
			return;
		case DISPLAY:
			instruction_register = 0xef;	//RST 05
			return;
		case NO_INTERRUPT: 
		default:
			break;
		};		
	}

	//printf("Current pc: %2x\n", pc);
	instruction_register = memory[pc];
	pc++;
}

void GetProgram()
{
	char buffer[8] = {0}; 		//stores string version of instruction

	char* ptr = NULL; 		//parameter for strtol

	uint8_t num = 0, 		//final int version of instruction
		not_finished = 1,	//bool indicating if user is done entering input
		no_memory_overflow = 1;	//bool indicating if user has used up available memory

	uint32_t byte_count = 0,	//number of bytes left for current starting address
		 address = 0x0000;	//address at which current byte is placed

	printf("First input must be number of bytes and starting address.\nFollowing inputs mut be 2 digit hex numbers until byte count is reached.\nEnter \"fi\" to finish input.\n");

	do
	{
		buffer[0] = 0;
		buffer[1] = 0;
		buffer[2] = 0;
		buffer[3] = 0;
		buffer[4] = 0;
		buffer[5] = 0;
		buffer[6] = 0;
		buffer[7] = 0;			

		if(byte_count > 0)
		{	
			printf("Address %i/%i: ", address, MEMORY_SIZE-1);

			//if the second value in the buffer is 0, than only one character was entered
			if(scanf("%2s", buffer) != 1 || buffer[1] == 0)
			{
				printf("Invalid Input. The expected input is a 2 digit hex value.\nTry again. ");
				continue;
			}
		} 
		else 
		{
			printf("Byte Count and Starting Address: ");

			//provide next byte count and starting address
			if(scanf("%8s", buffer) != 1 || (strcmp(buffer, "fi") != 0 && buffer[7] == 0))
			{
				printf("Invalid Input. The expected input is a byte count and starting address.\nTry again. ");
				continue;
			}
		}
			
		if(strcmp(buffer, "fi") == 0)
		{
			not_finished = 0;
			continue;
		}

		if(address >= MEMORY_SIZE)
		{
			no_memory_overflow = 0;
			continue;
		}

		//check if 2 char input is a hex value
		if(buffer[7] == 0)
		{
			if(!((buffer[0] >= '0' && buffer[0] <= '9') || (buffer[0] >= 'a' && buffer[0] <= 'f') || (buffer[0] >= 'A' && buffer[0] <= 'F')) 
			|| !((buffer[1] >= '0' && buffer[1] <= '9') || (buffer[1] >= 'a' && buffer[1] <= 'f') || (buffer[1] >= 'A' && buffer[1] <= 'F')))
			{
				printf("Invalid input. Each input must be a 2 digit hex value\nTry again: ");
				continue;
			}

			num = (uint8_t)strtol(buffer, &ptr, 16);
		
			memory[address] = num;

			byte_count--;
			address++;
		}
		//get 16-bit byte_count and 16-bit address 
		else 
		{
			byte_count = (strtol(buffer, NULL, 16) >> 16) & 0x00ffff;
			address = strtol(buffer, NULL, 16) & 0x00ffff;				

			printf("Byte Count: %i \nAddress: %i \n", byte_count, address);

			if(address >= MEMORY_SIZE)
			{
				printf("Please provide a valid address.\nTry again. ");
			}

			if ((address + byte_count - 1) > MEMORY_SIZE)
			{
				printf("Byte count (and starting address) exceeds available memory.\nTry again. ");
			} 

			continue;
		}
	} 
	while(not_finished && no_memory_overflow);
}

void DisplayState()
{
	printf("Registers:\n");
	printf("B: %02x C: %02x D: %02x E: %02x H: %02x L: %02x\n",
		 b[0], c[0], d[0], e[0], h[0], l[0]);

	printf("Accumulator: %02x\n", a[0]);	

	printf("Status:\n");
	printf("PC: %04x SP: %04x Flags: %02x\n", pc, sp, status[0]);  

	printf("Storage:\n");
	printf("CTRL: %02x DATA: %02x ADDR:%02x%02x\n", memory[NV_MEM_CTRL_REG], memory[NV_MEM_DATA_REG], memory[NV_MEM_ADDR_HIGH], memory[NV_MEM_ADDR_LOW]);
}

int main()
{
	time = 0;
	halt_enable = 0;
	interrupt_request = 0;
	priority = 8;

	hard_disk = malloc(HARD_DISK_SIZE * sizeof(uint8_t));
	address_space = malloc(ADDRESSED_SPACE_SIZE * sizeof(uint8_t));
	memory = address_space + MEMORY_START_ADDRESS;
	video_memory = address_space + VIDEO_MEM_START_ADDRESS;
	io = malloc(PORTS * sizeof(uint8_t));

	pc = 0x0000;
	status[0] = 0;
	interrupt_vector = NO_INTERRUPT;

	//atexit(DisplayState);
	
	LoadNonVolatileMemory(hard_disk);

	GetProgram();

	//StartMonitor();
	
	memory[NV_MEM_CTRL_REG] = 0x02;

	
	
	while(!halt_enable)
	{
		//fetches interrupt vector or next-instruction-in-program to instruction register
		InterruptCheckAndInstructionFetch();

		//decode - execute - store
		instruction_set[instruction_register]			//calls the instruction-emulating function
			(&instruction_set_data[instruction_register]);	//passes references to data needed to carry out instruction

		//printf("Control Register: %x\n", memory[NV_MEM_CTRL_REG]);
		PrintMachineState();
		NonVolatileMemoryOperation();		 	
	}
	
	StoreNonVolatileMemory(hard_disk);

	StopMonitor();		
	DisplayState();

	free(memory);			
	memory = NULL;
	
	free(hard_disk);
	hard_disk = NULL;

	return EXIT_SUCCESS;
}

