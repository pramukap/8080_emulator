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
 * BITS:	4			3			2			1		0		
 * VALUE:	Interrupt-Handled Flag	Write-Request Flag	Read-Request Flag	Ready Flag	Interrupt-Enable
 * 0x3ffd   - Data Register
 * 0x3ffe/f - Address Registers 
 */

void NonVolatileMemoryOperation()
{
	static int storage_op_completion_time = INT_MAX;
	static io_state storage_state = READY;
	uint16_t address;

	switch(storage_state)
	{
		case READY: 
				if(memory[NV_MEM_CTRL_REG] & READ_REQUEST)
				{
					//state output
					storage_op_completion_time = time + STORAGE_ACCESS_PERIOD;
					memory[NV_MEM_CTRL_REG] &= ~RDY;

					//next state
					storage_state = READING;
				}
				else if(memory[NV_MEM_CTRL_REG] & WRITE_REQUEST)
				{
					//state output
					storage_op_completion_time = time + STORAGE_ACCESS_PERIOD;
					memory[NV_MEM_CTRL_REG] &= ~RDY;
					
					//next state
					storage_state = WRITING;
				}

					//otherwise state = READY
				break;
		case READING:
				if(time >= storage_op_completion_time)
				{
					//state output
					address = (memory[NV_MEM_ADDR_HIGH] << 8) + memory[NV_MEM_ADDR_LOW];
					memory[address] = memory[NV_MEM_DATA_REG];
					storage_op_completion_time = INT_MAX;

					memory[NV_MEM_CTRL_REG] |= DONE;
					//next state
					storage_state = OP_COMPLETE;

					if(memory[NV_MEM_CTRL_REG] & INTERRUPT_ENABLE)
					{
						interrupt_request = 1;
						interrupt_vector = STORAGE_READ;
						storage_state = INTERRUPT;
					}
				}
			
				break;
		case WRITING:
				if(time >= storage_op_completion_time)
				{
					//state output
					address = (memory[NV_MEM_ADDR_HIGH] << 8) + memory[NV_MEM_ADDR_LOW];
					hard_disk[address] = memory[NV_MEM_DATA_REG];

					memory[NV_MEM_CTRL_REG] |= DONE;
					storage_op_completion_time = INT_MAX;

					//next state
					storage_state = OP_COMPLETE;

					if(memory[NV_MEM_CTRL_REG] & INTERRUPT_ENABLE)
					{
						interrupt_request = 1;
						interrupt_vector = STORAGE_WRITE;
						storage_state = INTERRUPT;
					}
				}
				break;
		case INTERRUPT:
				if(memory[NV_MEM_CTRL_REG] & DONE)
				{
					//state output
					interrupt_request = 1;
					interrupt_vector = (memory[NV_MEM_CTRL_REG] & READ_REQUEST) ? STORAGE_READ : STORAGE_WRITE;

					//state = INTERRUPT 
				}
				else
				{
					//state output
					memory[NV_MEM_CTRL_REG] |= RDY;
					memory[NV_MEM_CTRL_REG] &= ~(WRITE_REQUEST | READ_REQUEST);
					
					//next state
					storage_state = READY;
				}
				break;
		case OP_COMPLETE:
				if((memory[NV_MEM_CTRL_REG] & DONE) == 0)
				{
					//state output
					memory[NV_MEM_CTRL_REG] |= RDY;
					memory[NV_MEM_CTRL_REG] &= ~(WRITE_REQUEST | READ_REQUEST);

					//next state
					storage_state = READY; 
				}
				break;
		default: 
				break;
	};
}

/*
void NonVolatileMemoryOperation()
{
	static int storage_op_completion_time = INT_MAX;
	uint16_t address;

	//if ready, start timer to emulate access-operation, and clear READY to indicate operation is ongoing
	if(memory[NV_MEM_CTRL_REG] & READY)
	{
		if(memory[NV_MEM_CTRL_REG] & WRITE_REQUEST || memory[NV_MEM_CTRL_REG] & READ_REQUEST)
		{
			storage_op_completion_time = time + STORAGE_ACCESS_PERIOD;	//current time + time needed for access operation
		
			memory[NV_MEM_CTRL_REG] &= ~(READY | INTERRUPT_HANDLED);
		}
	}
	//if emulated time of operation has passed, do the requested operation
	else if((memory[NV_MEM_CTRL_REG] & READY) == 0 && time >= storage_op_completion_time)
	{
		//send an interrupt signal if interrupts are enabled
		if(memory[NV_MEM_CTRL_REG] & ~INTERRUPT_ENABLE)
		{
			interrupt_request = 1;
		}

		//indicate operation is complete and device can carry out another operation
		memory[NV_MEM_CTRL_REG] |= READY;


		//actual read operation
		if(memory[NV_MEM_CTRL_REG] & READ_REQUEST)
		{
			address = (memory[NV_MEM_ADDR_HIGH] << 8) + memory[NV_MEM_ADDR_LOW];
			memory[address] = memory[NV_MEM_DATA_REG];

			memory[NV_MEM_CTRL_REG] &= ~READ_REQUEST;
			storage_op_completion_time = INT_MAX;
			return;
		}

		//actual write operation
		if(memory[NV_MEM_CTRL_REG] & WRITE_REQUEST)
		{
			address = (memory[NV_MEM_ADDR_HIGH] << 8) + memory[NV_MEM_ADDR_LOW];
			hard_disk[address] = memory[NV_MEM_DATA_REG];

			memory[NV_MEM_CTRL_REG] &= ~WRITE_REQUEST;
			storage_op_completion_time = INT_MAX;
			return;
		}

	} 
}
*/
