/****************************************
 * 8080_Assembler v0.0.0		*
 * Pramuka Perera			*
 * July 23, 2017			*
 * Assembler for Intel 8080 Processor	*
 * Supports: Labels			*
 ****************************************/

#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

#include "label_list.h"
#include "instruction_set.h"
#include "pseudo_instruction_set.h"
#include "line_token_array.h"
#include "buffer.h"
#include "output_list.h"

//#define	TAB	0x09
//#define	NEWLINE	0x0a

FILE *assembly_file 	= NULL;	//input source file
FILE *object_file 	= NULL;	//output containing loader directives and machine-code file
FILE *listing_file 	= NULL; //output file containing original code and machine code in a readable format

char *assembly_code 	= NULL;		//buffer for assembly code drawn from input file
token *lines 		= NULL;		//array of line tokens (see line_token_array.h for description of line_tokens)
buffer 	*b 		= NULL,		//reusable buffer that hold snippets of code
	*b1		= NULL;
label *labels 		= NULL;		//linked list of assembly code labels
output *final 		= NULL;		//array of opcodes + operands that are to be printed to stdout or to a file
buffer *object_code	= NULL;		//buffer for object code prior to storage in object file

int main(int argc, char *argv[])
{
	char 	c;			
	
	int	code_size 		= 0,	//# of characters in assembly_code (including newlines, spaces, etc)
		code_index 		= 0,	//index of current char in assembly_code buffer
		line_array_size		= 0,	//total number of tokens in the line array
		line_index 		= 0,	//index of current line of assembler file
		location_counter 	= 0,	//memory address at which current instruction is being placed
		byte_counter		= 0,	//count of bytes for every ORG address
		not_end			= 1;	//boolean value that is set to 0 when the END pseudo-instruction is detected

	output 	*o 	= NULL,		//used to access nodes from the output linked list
		*o1 	= NULL;

	label l;			//used to access labels from the label linked list
	instruction i;			//used to access instructions from the instruction set
	
	void	*temporary_ptr = NULL;	
	
	/*
	//obtain asm and object filenames
	if(argc < 3)
	{
		printf("You didn't give me an assembly file and a target file.\nI'm gonna leave now :P\n");
		exit(0);
	}
	*/

	//open asm, object, and listing files
	if(argc < 3)
	{
		printf("You didn't give me the necessary filenames.\n");
		exit(EXIT_FAILURE);
	}


	if((assembly_file = fopen(argv[1], "r")) == NULL)
	{
		printf("Failed to open assembly file.\n");
		exit(EXIT_FAILURE);
	}
	if((object_file = fopen(argv[2], "w")) == NULL)
	{
		printf("Failed to open object file.\n");
		exit(EXIT_FAILURE);
	}
	if((listing_file = fopen(strcat(argv[2],".list"), "w")) == NULL)
	{
		printf("Failed to open assembly file\n");
		exit(EXIT_FAILURE);
	}

	//get and store the source code from input file
	//NOTE: The assembly_code buffer is not null-terminated
	if((assembly_code = malloc(sizeof(char))) == NULL)
	{
		printf("Failed to allocate memory for code buffer.\n");
		exit(EXIT_FAILURE);
	}
	code_size++;

	while((c = fgetc(assembly_file)) != EOF)
	{
		assembly_code[code_index] = c;
		fprintf(listing_file, "%c", assembly_code[code_index]);		

		//make space for a new char
		temporary_ptr = assembly_code;
		if((assembly_code = realloc(assembly_code, (code_size + 1) * sizeof(char))) == NULL)
		{
			printf("No more memory to store line token. Please try again.\n");
			free(temporary_ptr);
			exit(EXIT_FAILURE);
		}

		code_index++;
		code_size++;
	}
	assembly_code[code_index] = '\0';

	//printf("Done storing code\n");

	//process the assembly_code buffer to produce an line token array (lines) and a linked list of labels (labels)
	//NOTE: all labels and lines are null-terminated
	//code_index = 0;

	b = NewBuffer();	//buffer to store snippets of code from the assembly_code buffer

	for(code_index = 0; code_index < code_size; code_index++)
	{
		c = assembly_code[code_index];
	
		//is there a label in this line
		if(c == ':')
		{	
			//last character of the label MUST be followed by the ':'
			if(assembly_code[code_index - 1] < '!' || assembly_code[code_index - 1] > '~')
			{
				printf("Invalid Label: %s\n", b -> str);
			}

			//cap the string with a null terminator	
			AddCharToBuffer(b, '\0');
			
			//Clear away whitespace from the front of the label	
			while((b -> str)[0] == ' ' || (b -> str)[0] == TAB)
			{
				ShiftBufferContentsLeft(b);
			}

			//First character of label should be a letter, '@', or '?'			
			if( 
		      	      ((b -> str)[0] > '\0' && (b -> str)[0] < '?') 
			||    ((b -> str)[0] > '@'  && (b -> str)[0] < 'a')
			||     (b -> str)[0] > 'z' 			   )
			{
				printf("Invalid Label: %s\n", b -> str);
				exit(EXIT_FAILURE);
			}		

			/*
			//The assembler will only store the first 5 chars of the label
			if(strlen(b -> str) > 5)
			{
				//printf("Warning: %s is greater than 5 characters\n", b -> str);
			}
			*/
		
			AddLabelNode(&labels, b -> str, 0x10000, line_index);			
			//printf("Label: %s\n", (labels -> last_node) -> label);
	
			//New buffer for the next label or line
			b = NewBuffer();

			continue;
		}
		
		//store the line
		if(c == NEWLINE || c == ';')
		{
			//cap the string with a null terminator
			AddCharToBuffer(b, '\0');
			
			//first char in the buffer should be a null or a lowercase letter	
			while(((b -> str)[0] > '\0' && (b -> str)[0] < 'a') ||  (b -> str)[0] > 'z' )
			{
				ShiftBufferContentsLeft(b);
			}
			
			AddLineToken(b -> str, b -> length, line_index, line_array_size, &lines);
			
			//printf("Line Token #%i: %s\n", line_index, lines[line_index].line);

			line_index++;
			line_array_size++;			

			//New Buffer for the next label or line
			b = NewBuffer();
			
			//throw away any comments on the current line	
			while(c == ';' && assembly_code[code_index] != NEWLINE)
			{
				code_index++;
			} 
						
			continue;
		}  		

		AddCharToBuffer(b, ToLower(c));	
	}

	//printf("Done processing code into labels and lines\n");

	//look for pseudo-instructions, instructions, and operands in lines and assign values to labels
	b1 = NewBuffer();

	for(line_index = 0; line_index < line_array_size && not_end; line_index++)
	{
		b -> length = strlen(lines[line_index].line);
		b -> str = malloc((b -> length) * sizeof(char));
		strcpy(b -> str, lines[line_index].line);
		
		//find pseudo-instruction
		switch(FindPseudoInstruction(b -> str))
		{
			case ORG:	
				while((b -> str)[0] == ' ' || (b -> str)[0] == TAB)
				{
					ShiftBufferContentsLeft(b);
				}
				
				AddOutputNode(strtol(b -> str, NULL, 16), "", ORG_ADDR, &final);
				
				location_counter += strtol(b -> str, NULL, 16);
				
				break;
			
			case EQU:
				//record the label			
				while((b -> str)[0] != ' ' && (b -> str)[0] != TAB)
				{
					AddCharToBuffer(b1, (b -> str)[0]);
					ShiftBufferContentsLeft(b);
				}
				
				//clear away the whitespace to isolate the number
				while((b -> str)[0] == ' ' || (b -> str)[0] == TAB)
				{
					ShiftBufferContentsLeft(b);
				}
				
				AddLabelNode(&labels, b1 -> str, strtol(b -> str, NULL, 16), -1);
				
				break;
			
			case END:
				//set a at_end boolean value to 1; exits for-loop
				not_end = 0;
				break;
			
			case NO_PSEUDO_FOUND:
			default:
				break;
		};		
		
		//assign label value
		AssignLabelValue(line_index, location_counter, labels);	

		//find and get the instruction in the line
		i = BinarySearch(b -> str);

		//an instruction was found	
		if(i.opcode != 0x20)
		{
			//update location_counter to where the next instruction is stored
			location_counter += (i.operand_bytes + 1);
			
			while((b -> str)[0] == ' ' || (b -> str)[0] == ',')
			{
				ShiftBufferContentsLeft(b);
			}
			
			//output node for the discovered instruction
			AddOutputNode(i.opcode, b -> str, i.type, &final); 
		}	
	}

	//printf("Done processing lines and finding label values.\n");

	//process operands into integer values; store opcodes and operands to files
	object_code = NewBuffer();
	o = final;

	while(o != NULL)
	{
		l = FindLabelInOperand(o -> operand, labels); 
		
		//found a label
		if(l.value != 0x10000)
		{
			o -> final_operand = l.value;
		}
		else
		{
			o -> final_operand = strtol(o -> operand, NULL, 16);
		}

		if(o -> type != ORG_ADDR)
		{	
			fwrite(&(o -> opcode), sizeof(uint8_t), 1, object_file); 	
			fprintf(listing_file, "%02x\n", o -> opcode);
		}
	
		//carry out final write task based on operand type	
		switch(o -> type)
			{
				case 	D8:
						//Do I need the casting here?
						AddCharToBuffer(object_code, (uint8_t)((o -> final_operand) & 0x0ff));
						//fwrite(&(o -> final_operand), sizeof(uint8_t), 1, object_file);
						fprintf(listing_file, "%02x\n", o -> final_operand);
						break;

				case	D16:	
				case	ADDR:
						
						//fwrite(&(o -> final_operand), sizeof(uint8_t), 2, object_file);
						//print the low byte
						AddCharToBuffer(object_code, (uint8_t)((o -> final_operand) & 0x0ff));
						fprintf(listing_file, "%02x\n", (o -> final_operand) & 0x0ff);
		
						//print the high byte
						AddCharToBuffer(object_code, (uint8_t)(((o -> final_operand) >> 8) & 0x0ff));
						fprintf(listing_file, "%02x\n", ((o -> final_operand) >> 8) & 0x0ff);
						break;
				
				case	ORG_ADDR:
						byte_counter = 0;
						o1 = o -> next;
						//count the bytes between this org and the next org
						while(o1 != NULL)
						{
							if(o1 -> type == ORG_ADDR)
							{
								break;
							}
							byte_counter++;
							o1 = o1 -> next;
						}

						AddCharToBuffer(object_code, (uint8_t)byte_counter);
						AddCharToBuffer(object_code, (uint8_t)((o -> opcode) & 0x0ff));
						AddCharToBuffer(object_code, (uint8_t)(((o -> opcode) >> 8) & 0x0ff));
						fprintf(listing_file, "%02x%04x\n", byte_counter, o -> opcode);
						break;
				
				case 	NONE: 
					default:
						//printf("\n"); 
						break;
			};
		
		o = o -> next;
	}	
	
	fprintf(listing_file, "fi\n");

	//close files and free memory
	fclose(assembly_file);
	assembly_file = NULL;

	fclose(object_file);
	object_file = NULL;

	fclose(listing_file);
	listing_file = NULL;

	if(assembly_code != NULL)
	{
		free(assembly_code);
		assembly_code = NULL;
	}

	FreeLabelList(&labels);

	FreeLineTokens(line_array_size, &lines);
	
	FreeBuffer(&b);	
	
	FreeOutputList(&final);
	
	return EXIT_SUCCESS;
}

