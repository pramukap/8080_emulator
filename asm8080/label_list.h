/********************************************************************************
 * Label List Object for the 8080 Assembler					*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * A place to put all the assembly labels (data and address labels)		* 
 ********************************************************************************/

#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

typedef struct label_node
{
	char *label;				//stores null-terminated label
	int value;				//value associated with label
	int index;				//for address labels, line on which label was found
	struct label_node *next;
	struct label_node *last;
}
label;

int AddLabelNode(label **head, char *new_label, int new_value, int line_index)
{
	label *new_node;

	if((new_node = malloc(sizeof(label))) == NULL)
	{
		printf("Couldn't allocate space to save a label node.\n");
		return EXIT_FAILURE;
	}

	if(((new_node -> label) = malloc((strlen(new_label) + 1) * sizeof(char))) == NULL)
	{
		printf("Failed to allocate space for label.\n");
		return EXIT_FAILURE;
	}

	strcpy(new_node -> label, new_label);	
	new_node -> value = new_value;
	new_node -> index = line_index; 
	new_node -> next = NULL;
	new_node -> last = NULL;	

	if(*head == NULL) 
	{
		(*head) = new_node;
		(*head) -> last = *head;

		return EXIT_SUCCESS;
	}

	((*head) -> last) -> next = new_node;
	(*head) -> last = new_node;

	return EXIT_SUCCESS;
}

int AssignLabelValue(int line_index, int value_to_assign, label *head)
{
	label *label_node = head;

	while(label_node != NULL)
	{
		if(line_index == label_node -> index)
		{
			label_node -> value = value_to_assign;
			return 1;
		}

		label_node = label_node -> next;
	}

	return 0;
}

label FindLabelInOperand(char *operand, label *head)
{
	int	i,					//index from which to start looking for the label in the operand
		label_length,
		operand_length = strlen(operand),
		last_char_index = strlen(operand) - 1,	//the last char in the operand
		label_check = 0;

	char	char_after_label,			//if a label is in the operand, this is the char that comes after it
		char_before_label;			//if a label is in the operand, this is the char that comes before it

	label 	*label_node = head,
		no_label_found = {"no l", 0x10000, -1, NULL, NULL}; 

	while(label_node != NULL)
	{
		label_length = strlen(label_node -> label);
		
		//try to find the current label somewhere in this operand
		for(i = 0; i + label_length <= operand_length; i++)
		{
			//was the label found in the operand
			if(strncmp(operand + i, label_node -> label, label_length) == MATCH)
			{
				//prevent following type of case: Identified Label: "n", Actual Label: "noob_noob"
				if(i + label_length <= last_char_index)
				{
					char_after_label = (operand + i + label_length)[0];
				
					if(char_after_label == ' ' || char_after_label == TAB)
					{
						label_check++;	
					}
				}
				else
				{
					label_check++;
				}

				//prevent following type of case: Identified Label: "abed", Actual Label: "evil_abed"
				if(i > 0)
				{
					char_before_label = (operand + i - 1)[0];
	
					if(char_before_label == ' ' || char_after_label == TAB)
					{
						label_check++;
					}
				}
				else
				{
					label_check++;
				}

				if(label_check == 2)
				{
					return *label_node;
				}

				label_check = 0;
			}
		}

		label_node = label_node -> next;
	}

	return no_label_found;
}

void FreeLabelList(label **head)
{
	label 	*current_node,
		*next_node;

	if(*head == NULL)
	{
		return;
	}
	
	current_node = *head;

	while(current_node != NULL)
	{
		next_node = current_node -> next;
		free(current_node);
		current_node = next_node;
	}

	*head = NULL;
}

