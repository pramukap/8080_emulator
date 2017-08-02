/****************************************
 * 8080_Assembler v0.0.0		*
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
#include "buffer.h"
#include "output_list.h"

#define	NEWLINE	0x0a

FILE *assembly_file;	//input file
//FILE *object_file;	//output file

node *labels;		//linked list of labels
token *lines;		//array of line tokens


int main(int argc, char *argv[])
{
	char 	c,
		*assembly_code = NULL;	//container for assembly code drawn from file
	
	int 	code_size 	= 1,	//# of characters in assembly code (including newlines, spaces, etc)
		code_index 	= 0,	//index of current char of assembly code
		num_line_tokens	= 0,	//number of tokens in the line array
		line_index 	= 0;	//index of current line of assembler file

	uint16_t location_counter = 0;	//base memory address at which program is being placed
	
	uint8_t input_value;		//holds value that is to be placed in object code file

	buffer *b = NULL;

	instruction i;
	
	void	*temporary_ptr = NULL;	
	
	//obtain asm and object filenames
	if(argc < 3)
	{
		printf("You didn't give me an assembly file and a target file.\nI'm gonna leave now :P\n");
		exit(0);
	}

	assembly_file = fopen(argv[1], "r");
	//object_file = fopen(argv[2], "w");

	//receive and store code from assembly source
	assembly_code = malloc(sizeof(char));

	while((c = fgetc(assembly_file)) != EOF)
	{
		assembly_code[code_index] = c;
		//printf("%c", assembly_code[code_index]);		

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

	//process code-buffer to produce (array of line tokens) and (linked list of labels)
	code_index = 0;

	b = NewBuffer();

	while(code_index < code_size)
	{
		c = assembly_code[code_index];
	
		//if buffer contains label
		if(c == ':')
		{
			//add null terminator to string	
			AddCharToBuffer(b, '\0');
			
			//while((b -> str)[0] < 'a' || (b -> str)[0] > 'z' || (b -> str)[0] != '\0')
			//while the first char in the buffer is not null or a lowercase letter	
			while(((b -> str)[0] > '\0' && (b -> str)[0] < 'a') ||  (b -> str)[0] > 'z' )
			{
				ShiftBufferContentsLeft(b);
			}
		
			AddLabelNode(b -> str, line_index, &labels);			
			//printf("Label: %s\n", (labels -> last_node) -> label);
	
			//reset buffer
			b = NewBuffer();

			code_index++;
			continue;
		}
		
		//if at end of line
		if(c == NEWLINE || c == ';')
		{
			//add null terminator to string	
			AddCharToBuffer(b, '\0');
			
			//while the first char in the buffer is not null or a lowercase letter	
			while(((b -> str)[0] > '\0' && (b -> str)[0] < 'a') ||  (b -> str)[0] > 'z' )
			{
				ShiftBufferContentsLeft(b);
			}

			
			AddLineToken(b -> str, b -> length, line_index, &num_line_tokens, &lines);
			
			//printf("Line Token #%i: %s\n", line_index, lines[line_index].line);

			line_index++;
			num_line_tokens += 1;			

			//reset buffer
			b = NewBuffer();
			
			//if ; then go to new line	
			while(c == ';' && assembly_code[code_index] != NEWLINE)
			{
				code_index += 1;
			} 
						
			code_index++;
			continue;
		}  		

		AddCharToBuffer(b, ToLower(c));	

		code_index++;
	}

	//code_index =  (uint16_t)BinarySearch("mov e,a");
	//printf("Result of Binary Search: %x\n", code_index);  

	//process lines
	for(line_index = 0; line_index < num_line_tokens; line_index++)
	{
		//put line in a buffer
		b -> str = lines[line_index].line;	
		b -> length = strlen(lines[line_index].line);

		//find pseudo-instruction
		
		//assign label value
		AssignLabelValue(line_index, location_counter, labels);	

		//find instruction
		i = BinarySearch(b -> str);
		printf("%02x\n", i.opcode);
		//putw(i.opcode, object_file);		

		//clear away empty spaces to reach possible operands	
		if(i.opcode != 0x20)
		{
			//add space occupied by instruction to location_counter
			location_counter += (i.operand_bytes + 1);
			
			while((b -> str)[0] == ' ')
			{
				ShiftBufferContentsLeft(b);
			}
			

			//find and process operand
			switch(i.operand_2_type)
			{
				case 	D8:
						printf("%s\n", b -> str);
						break;
				case	D16:	
						printf("%c%c\n%c%c\n", (b -> str)[2], (b -> str)[3], (b -> str)[0], (b -> str)[1]);
						break;
				case	ADDR:
						printf("%c%c\n%c%c\n", (b -> str)[2], (b -> str)[3], (b -> str)[0], (b -> str)[1]);
						break;
				case	R:
				case	RP:
				case 	NONE: 
					default:
						//printf("\n"); 
						break;
			};

			
		}

			
	}

	printf("fi\n");

	//free memory
	
	return 1;
}

