/********************************************************************************
 * Line Token Array Object for the 8080 Assembler				*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * A line token stores each line of assembly code, excluding any labels		*
 * and comments in that line							*
 ********************************************************************************/


#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>

	#define INCLUDE
#endif

typedef struct l_token
{
	char *line;
	int line_length;
}
token;

void AddLineToken(char* new_line, int new_line_length, int line_index, int num_tokens, token **array)
{
	token *temporary_ptr = *array;

	if(*array == NULL)
	{
		if((*array = malloc(sizeof(token))) == NULL)
		{
			printf("Insufficient memory for line token.");
			exit(0);			
		}

		(*array)[line_index].line = new_line;
		(*array)[line_index].line_length = new_line_length;

		return;
	}
	else if((*array = realloc(*array, (num_tokens + 1) * sizeof(token))) == NULL)
	{
		printf("Insufficient memory for line token.\n");
		//Need to free all memory before exiting
		exit(0);
	}
	
	(*array)[line_index].line = new_line;
	(*array)[line_index].line_length = new_line_length;
}

void FreeLineTokens(int num_tokens, token **array)
{
	int i;

	if(*array == NULL)
	{
		return;
	}

	//free the line strings in each token
	for(i = 0; i < num_tokens; i++)
	{
		free((*array)[i].line);
	}

	free(*array);
	*array = NULL;
}

