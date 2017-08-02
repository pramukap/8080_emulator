/********************************************************************************
 * Label List Object for the 8080 Assembler					*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * A place to put all the labels in an assembly program				* 
 ********************************************************************************/

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
	int value;
	int line_index;
	struct list_node *next_node;
	struct list_node *last_node;
}
node;

void AddLabelNode(char *new_label, int index, node **head)
{
	node 	*new_node;

	if((new_node = malloc(sizeof(node))) == NULL)
	{
		printf("Couldn't allocate space to save a label node.");
		exit(0);
	}

	strncpy(new_node -> label, new_label, sizeof(new_node -> label));	
	new_node -> value = 0x10000;
	new_node -> line_index = index; 
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

int AssignLabelValue(int line_index, int value_to_assign, node *head)
{
	node *label = head;

	while(label != NULL)
	{
		if(line_index == label -> line_index)
		{
			label -> value = value_to_assign;
		}

		label = label -> next_node;
		return 1;
	}

	return 0;
}

int FindLabelValue(char *label_of_address, node *head)
{
	node *current_node = head;
	
	while(current_node)
	{
		if(strcmp(current_node -> label, label_of_address) == 0)
		{
			return (current_node -> value);
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

