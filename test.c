#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 4
#define INPUT_SIZE (MEMORY_SIZE + 1)

struct instruction_data
{
	uint16_t a;
	uint8_t b;
}
data;

uint8_t register_file[10];

uint8_t *reg1 = register_file + 0;
uint16_t *reg1_pair = (uint16_t *)(register_file + 0);

typedef int (*instruction)(uint8_t *a, uint16_t *b);

int test(uint8_t *c, uint16_t *d)
{
	printf("Works\n");
	return 2;	
}

int test2(uint8_t *c, uint16_t *d)
{
	printf("Works\n");
	return 3;	
}

//Test of code that will receive and store 8080 machine code to memory
/*
 *Test Cases:
 *Input:	Expected Output:
 *0		bad input
 *1		bad input
 *00		0
 *01		1
 *10		16
 *a		bad input
 *0a		10
 *fa		250
 *ag		bad input
 *ga		bad input
 *1a		26
 *1A		26
 *fA		250
 *Fa		250
 */
void test3()
{
	char buffer[2] = {0}; //stores string version of instruction
	char* ptr = NULL; 	//parameter for strtol
	uint8_t num = 0, 	//final int version of instruction
		not_finished = 1,
		no_memory_overflow = 1;
	uint32_t i = 0;

	printf("All inputs mut be 2 digit hex numbers.\nEnter \"fi\" to finish input.\n");

	//print current memory address or current index out of total indices
	do{
		buffer[0] = 0;
		buffer[1] = 0;

		printf("Memory Space %i/%i: ", i + 1, MEMORY_SIZE);

		if(scanf("%2s", buffer) != 1 || buffer[1] == 0) //if the second value in the buffer is 0, than only one character was entered
		{
			printf("Invalid Input. Each input must be a 2 digit hex value.\nTry again: ");
			continue;
		}

		if(strcmp(buffer, "fi") == 0)
		{
			not_finished = 0;
			continue;
		}

		if(i >= MEMORY_SIZE)
		{
			no_memory_overflow = 0;
			continue;
		}

		printf("|%i %i|\n", buffer[0], buffer[1]);

		if(!((buffer[0] >= '0' && buffer[0] <= '9') || (buffer[0] >= 'a' && buffer[0] <= 'f') || (buffer[0] >= 'A' && buffer[0] <= 'F')) 
		|| !((buffer[1] >= '0' && buffer[1] <= '9') || (buffer[1] >= 'a' && buffer[1] <= 'f') || (buffer[1] >= 'A' && buffer[1] <= 'F')))
		{
			printf("Invalid input. Each input must be a 2 digit hex value\nTry again: ");
			continue;
		}

		//if i < MEMORY_SIZE put num in memory
		num = (uint8_t)strtol(buffer, &ptr, 16);
		printf("%i\n", num);
		i++;
	}
	while(not_finished && no_memory_overflow);

	if(!not_finished)
	{
		printf("Program saved to memory. Ready for execution.\n");
	}
	else if(!no_memory_overflow)
	{
		printf("Program is too large to fit in memory.\n");
		exit(0);
	}
}

instruction instruction_set[2] = {test, test2};

int main()
{	
	int i = 0;
	int c = 0x80;
	
	i = instruction_set[1](reg1, reg1_pair);
	printf("%i\n", i);

	printf("%i\n", c>>7);

	test3();
}

