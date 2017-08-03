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

typedef struct label_node
{
	char label[6];
	int value;
	int line_index;
	struct label_node *next_node;
	struct label_node *last_node;
}
label;

void AddLabelNode(char *new_label, int index, label **head)
{
	label *new_node;

	if((new_node = malloc(sizeof(label))) == NULL)
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

int AssignLabelValue(int line_index, int value_to_assign, label *head)
{
	label *label_node = head;

	while(label_node != NULL)
	{
		if(line_index == label_node -> line_index)
		{
			label_node -> value = value_to_assign;
			return 1;
		}

		label_node = label_node -> next_node;
	}

	return 0;
}

label FindLabelInOperand(char *operand, label *head)
{
	int	i,
		label_length,
		result,
		operand_length = strlen(operand),
		last_operand_index = strlen(operand) - 1;

	char	char_after_label;
		//*front_buffer,
	 	//*back_buffer;

	label 	*label_node = head,
		no_label_found = {"no l", 0x10000, -1, NULL, NULL}; 

	while(label_node != NULL)
	{
		label_length = strlen(label_node -> label);
		
		for(i = 0; i + label_length < operand_length; i++)
		{
			if((result = strncmp(operand + i, label_node -> label, label_length)) == 0)
			{
				//want to make sure the label I have identified is the correct label, and not one that is contained in another
				//example: Identified Label: "b", Actual Label: "butt"
				if(i + label_length <= last_operand_index)
				{
					char_after_label = (operand + i + label_length)[0];
				
					/*	
					if( char_after_label < '0' 
					|| (char_after_label > 9 && char_after_label < 'a') 
					||  char_after_label > 'z') 
					*/
					if(char_after_label == ' ')
					{
						return *label_node;
					}
				}
				else
				{
					return *label_node;
				}
				
				//trying to replace label with value string
				/*
				front_buffer = calloc((i+1) * sizeof(char));
				//front_buffer[i] = '\0';

				back_buffer = calloc((operand_length - label_length - i + 1) * sizeof(char));
				
				//copy everything before the label into front_buffer
				strncpy(front_buffer, operand, i);
				//copy everything after the label into this buffer
				strcpy(back_buffer, operand + i + label_length); 

				
				operand_length = strlen(operand);
				i = 0;
				*/
			}
		}

		label_node = label_node -> next_node;
	}

	return no_label_found;
}

/*
int FindLabelValue(char *label_of_address, label *head)
{
	label *current_node = head;
	
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
*/

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
		next_node = current_node -> next_node;
		free(current_node);
		current_node = next_node;
	}

	*head = NULL;
}

