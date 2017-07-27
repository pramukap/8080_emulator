/****************************************
 * 8080_Assembler			*
 * Pramuka Perera			*
 * July 23, 2017			*
 * Assembler for Intel 8080 Processor	*
 ****************************************/

#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>

	#define INCLUDE
#endif

#include "label_list.h"
#include "instruction_set.h"
#include "line_token_array.h"

#define	NEWLINE	0x0a

/*
inline void SafeMalloc(void **ptr, int amount_to_allocate, char *failure_message)
{
	if((*ptr = malloc(amount_to_allocate * sizeof(**ptr))) == NULL)
	{
		printf("%s", failure_message);
	}
} 
*/

FILE *assembly_file;
FILE *object_file;

node *labels;		//linked list of labels
token *lines;		//array of line tokens


int main(int argc, char *argv[])
{
	char 	c,
		*buffer = NULL,
		*assembly_code = NULL;
	void	*temporary_ptr = NULL;
	
	int 	code_size 	= 1,
		code_index 	= 0,
		buffer_size 	= 1,
		buffer_index 	= 0,
		num_line_tokens	= 1,
		line_index 	= 0;

	uint16_t input_address	= 0;

	//obtain asm and object filenames
	if(argc < 3)
	{
		printf("You didn't give me an assembly file and a target file.\nI'm gonna leave now :P\n");
		exit(0);
	}

	assembly_file = fopen(argv[1], "r");
	object_file = fopen(argv[2], "w");

	//receive and store code from assembly source
	assembly_code = malloc(sizeof(char));

	while((c = fgetc(assembly_file)) != EOF)
	{
		assembly_code[code_index] = c;
		printf("%c", assembly_code[code_index]);		

		//if realloc fails, exit
		temporary_ptr = assembly_code;
		if((assembly_code = realloc(assembly_code, (code_size + 1) * sizeof(char))) == NULL)
		{
			printf("No more memory to store line token. Please try again.\n");
			free(temporary_ptr);
			exit(0);
		}

		code_index++;
		code_size++;
	}

	//process code-buffer to produce array of strings of line tokens and linked list of labels
	code_index = 0;

	lines = malloc(sizeof(token));
	buffer = malloc(sizeof(char));
	
	while(code_index < code_size)
	{
		c = assembly_code[code_index];
	
		//if buffer contains label
		if(c == ':')
		{
			AddLabelNode(buffer, input_address + (uint16_t)line_index, &labels);			
			printf("Label: %s\n", (labels -> last_node) -> label);
	
			//reset buffer
			temporary_ptr = buffer;
			if((buffer = realloc(buffer, sizeof(char))) == NULL)
			{
				printf("No more memory to store program. Please try again.\n");
				free(temporary_ptr);
				exit(0);
			}		

			buffer_index = 0;
			buffer_size = 1;

			code_index++;
			continue;
		}
		
		//if at end of line
		if(c == NEWLINE || c == ';')
		{
			//create new line token
			AddLineToken(buffer, buffer_size, line_index, &num_line_tokens, &lines);
			//lines[line_index].line = buffer;
			//lines[line_index].line_length = buffer_size;
			
			printf("Line Token #%i: %s\n", line_index, lines[line_index].line);

			//add new token struct
			/*temporary_ptr = lines;
			if((lines = realloc(lines, (num_line_tokens + 1) * sizeof(token))) == NULL)
			{
				printf("No more memory to store program. Please try again.\n");
				free(temporary_ptr);
				exit(0);
			}
			lines[line_index + 1].line = NULL; 
			*/

			line_index += 1;
			//num_line_tokens += 1;			

			//reset buffer
			temporary_ptr = buffer;
			if((buffer = malloc(sizeof(char))) == NULL)
			{
				printf("Insufficient space to allocate buffer.");
				//free all allocated memory
				exit(0);
			}
				
	
			//if ; then go to new line	
			while(c == ';' && assembly_code[code_index] != NEWLINE)
			{
				code_index += 1;
			} 
			
			buffer_index = 0;
			buffer_size = 1;
			
			code_index++;
			continue;
		}  		

		buffer[buffer_index] = c;	
		buffer_index++;
		buffer_size++;

		code_index++;
	}

	/*
	SafeMalloc(&buffer, 2, "Buffer was not allocated");
	buffer[0] = 'h';
	buffer[1] = 'i';
	printf("%s\n", buffer);
	*/

	//process operands

	//free memory
	
	return 1;
}

