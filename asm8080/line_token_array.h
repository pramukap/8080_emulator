/********************************************************************************
 * Line Token Array Object for the 8080 Assembler				*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * Line token array and corresponding functions to be used by assembler.c	*
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

char *GetLineToken(int token_index, token *array)
{
	return array[token_index].line;
}

int GetTokenLength(int token_index, token *array)
{
	return array[token_index].line_length;
}

void AddLineToken(char* _line, int _line_length, int line_index, int *num_tokens, token **array)
{
	token *temporary_ptr = *array;

	if(*array == NULL)
	{
		if((*array = malloc(sizeof(token))) == NULL)
		{
			printf("Insufficient memory for line token.");
			exit(0);			
		}

		(*array)[line_index].line = _line;
		(*array)[line_index].line_length = _line_length;
		*num_tokens += 1;

		return;
	}
	
	if((*array = realloc(*array, (*num_tokens + 1) * sizeof(token))) == NULL)
	{
		printf("Insufficient memory for line token.\n");
		//Need to free all memory before exiting
		exit(0);
	}
	
	(*array)[line_index].line = _line;
	(*array)[line_index].line_length = _line_length;
	*num_tokens += 1;
}

void FreeLineTokens(int num_tokens, token **array)
{
	int i;

	if(*array == NULL)
	{
		return;
	}

	while(i < num_tokens)
	{
		free((*array)[i].line);
		i++;
	}

	free(*array);
	*array = NULL;
}

