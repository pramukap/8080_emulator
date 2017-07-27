#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	
	#define INCLUDE 
#endif

typedef struct list_node
{
	char label[5];
	uint16_t address;
	struct list_node *next_node;
	struct list_node *last_node;
}
node;

void AddLabelNode(char *new_label, uint16_t new_address, node **head)
{
	node 	*new_node;

	if((new_node = malloc(sizeof(node))) == NULL)
	{
		printf("Couldn't allocate space to save a label node.");
		exit(0);
	}

	strncpy(new_node -> label, new_label, sizeof(new_node -> label));	
	new_node -> address = new_address;
	new_node -> next_node = NULL;
	new_node -> last_node = NULL;	

	if(*head == NULL) 
	{
		(*head) = new_node;
		(*head) -> last_node = *head;

		return;
	}

	((*head) -> last_node) -> next_node = new_node;
	(*head) -> last_node = new_node;
}

uint32_t FindLabelValue(char *label_of_address, node *head)
{
	node *current_node = head;
	
	while(current_node)
	{
		if(strcmp(current_node -> label, label_of_address) == 0)
		{
			return (current_node -> address);
		}

		current_node = current_node -> next_node; 
	}

	return 0x10000;	
}

void FreeLabelList(node **head)
{
	node 	*current_node,
		*next_node;

	if(*head == NULL)
	{
		return;
	}
	
	current_node = *head;

	while(current_node != NULL)
	{
		next_node = current_node -> next_node;
		free(current_node);
		current_node = next_node;
	}

	*head = NULL;
}

