/************************************************************************
 * 8080 Emulator Storage Peripheral v0.0.0				*
 * Pramuka Perera							*
 * June 23, 2017							*
 * Non-volatile data storage					 	*
 ************************************************************************/

#ifndef INCLUDE
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

#include <limits.h>

#define STORAGE_ACCESS_RATE	40	//Hz (25 ms)
#define STORAGE_ACCESS_PERIOD	(CLOCK_RATE / STORAGE_ACCESS_RATE) //Clock Cycles

#define INTERRUPT_ENABLE	0x01
#define READY			0x02
#define READ_REQUEST		0x04
#define WRITE_REQUEST		0x08

FILE *storage = NULL;

void LoadNonVolatileMemory(uint8_t *hard_disk)
{
	int i;
	
	storage = fopen("storage", "r");

	for(i = 0; i < HARD_DISK_SIZE; i++)
	{
		hard_disk[i] = fgetc(storage);
	}

	fclose(storage);
	storage = NULL;
}

void StoreNonVolatileMemory(uint8_t *hard_disk)
{
	int i;

	storage = fopen("storage", "w");

	for(i = 0; i < HARD_DISK_SIZE; i++)
	{
		fputc(hard_disk[i], storage);
	}	

	fclose(storage);
	storage = NULL;
}


/*
 * Memory-mapped Nonvolatile Memory Registers
 * 0x3ffc   - Storage Control Register
 * BITS:	3			2			1		0		
 * VALUE:	Write-Request Flag	Read-Request Flag	Ready Flag	Interrupt-Enable
 * 0x3ffd   - Data Register
 * 0x3ffe/f - Address Registers 
 */
void NonVolatileMemoryOperation()
{
	static int operation_completion_time = INT_MAX;
	uint16_t address;

	//if ready, start timer to emulate access-operation, and clear READY to indicate operation is ongoing
	if(memory[NV_MEM_CTRL_REG] & READY)
	{
		if(memory[NV_MEM_CTRL_REG] & WRITE_REQUEST || memory[NV_MEM_CTRL_REG] & READ_REQUEST)
		{
			operation_completion_time = time + STORAGE_ACCESS_PERIOD;	//current time + time needed for access operation
		
			memory[NV_MEM_CTRL_REG] &= ~READY;
		}
	}
	//if emulated time of operation has passed, do the requested operation
	else if((memory[NV_MEM_CTRL_REG] & READY) == 0 && time >= operation_completion_time)
	{
		if(memory[NV_MEM_CTRL_REG] & ~INTERRUPT_ENABLE)
		{
			interrupt_request = 1;
		}

		memory[NV_MEM_CTRL_REG] |= READY;


		if(memory[NV_MEM_CTRL_REG] & READ_REQUEST)
		{
			address = (memory[NV_MEM_ADDR_HIGH] << 8) + memory[NV_MEM_ADDR_LOW];
			memory[address] = memory[NV_MEM_DATA_REG];

			memory[NV_MEM_CTRL_REG] &= ~READ_REQUEST;
			operation_completion_time = INT_MAX;
			return;
		}

		if(memory[NV_MEM_CTRL_REG] & WRITE_REQUEST)
		{
			address = (memory[NV_MEM_ADDR_HIGH] << 8) + memory[NV_MEM_ADDR_LOW];
			hard_disk[address] = memory[NV_MEM_DATA_REG];

			memory[NV_MEM_CTRL_REG] &= ~WRITE_REQUEST;
			operation_completion_time = INT_MAX;
			return;
		}

	} 
}

