/********************************************************************************
 * 8080 Output List Object for the 8080 Assembler				*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * 8080's instruction set:							*
 * Used by the assembler to store the final output values prior to actual output*
 ********************************************************************************/

#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

typedef struct output_node
{
	int opcode;
	char *operand;			//stores the unprocessed operand
	int final_operand;		//stores the final numerical value of the operand
	operand_type type;		//data byte, data word, or address
	struct output_node *next;
	struct output_node *last;
}
output;

int AddOutputNode(int new_opcode, char *new_operand, operand_type new_operand_type, output **head)
{
	output *new_node;

	if((new_node = malloc(sizeof(output))) == NULL)
	{
		printf("Failed to allocate output node.\n");
		
		return EXIT_FAILURE;
	}

	new_node -> opcode = new_opcode;
	new_node -> final_operand = 0x10000;
	new_node -> type = new_operand_type;
	new_node -> next = NULL;
	new_node -> operand = NULL;
	
	//allocate space for new_operand including null terminator
	if((new_node -> operand = malloc((strlen(new_operand) + 1) * sizeof(char))) == NULL)
	{
		printf("Failed to allocate string.\n");
		
		return EXIT_FAILURE;
	}
	strcpy(new_node -> operand, new_operand);
	
	if(*head == NULL)
	{
		*head = new_node;	
		new_node -> last = *head;
		
		return EXIT_SUCCESS;
	}	

	((*head) -> last) -> next = new_node;
	(*head) -> last = new_node;

	return EXIT_SUCCESS; 
}

void FreeOutputList(output **head)
{
	output 	*current_node,
		*next_node;

	if(*head == NULL)
	{
		return;
	}

	current_node = *head;

	while(current_node != NULL)
	{
		next_node = current_node -> next;
		free(current_node -> operand);
		free(current_node);
		current_node = next_node;
	}
	
	*head = NULL;
}
