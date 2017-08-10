/************************************************************************
 * Buffer Objcet for 8080_Assembler					*
 * Pramuka Perera							*
 * July 28, 2017							*
 * Place to store characters from the assembly code while they're	*
 * being processed into useful chunks					*
 ************************************************************************/

#ifndef INCLUDE
	#include <stdlib.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

typedef struct str_buffer
{
	char *str;
	int last_char_index;
	int length;
}
buffer;

//Creates an empty buffer
buffer *NewBuffer(void)
{
	buffer *b;

	if((b = malloc(sizeof(buffer))) == NULL)
	{
		printf("Insufficient memory to allocate a buffer.\n");
		exit(EXIT_FAILURE);
	}

	/*
	if(((b -> str) = malloc(sizeof(char))) == NULL)
	{
		printf("Insufficient memory to allocate a buffer string.\n");
		free(b);
		exit(0);
	}
	*/

	b -> str = NULL;
	b -> last_char_index = -1;
	b -> length = 0;

	return b;
}

void AddCharToBuffer(buffer *b, char c)
{
	buffer *temporary_ptr = b;

	if(b -> str == NULL)
	{
		if(((b -> str) = malloc(sizeof(char))) == NULL)
		{
			printf("Insufficient memory to allocate a buffer string.\n");
			free(b);
			exit(EXIT_FAILURE);
		}
	}
	else if((b -> str = realloc(b -> str, ((b -> length) + 1) * sizeof(char))) == NULL)
	{
		printf("Insufficient memory to allocate a buffer.\n");
		//free allocated memory
		free(temporary_ptr);
		exit(EXIT_FAILURE);
	}

	(b -> last_char_index)++;

	(b -> str)[b -> last_char_index] = c;	

	(b -> length)++;
	//(b -> index)++;
}

//Converts uppercase letter to a lowercase letter
char ToLower(char c)
{
	if(c >= 'A' && c <= 'Z')
	{
		c += 0x20;
	}
	
	return c;
}

//Shifts the contents of the buffer to the left by one
//and places null terminators in the empty spaces to the right
void ShiftBufferContentsLeft(buffer *b)
{
	int i;

	for(i = 1; i < (b -> length); i++)
	{
		(b -> str)[i-1] = (b -> str)[i];
	}

	(b -> str)[(b -> length) - 1] = '\0';
	(b -> length)--;
	(b -> last_char_index)--;
}

void FreeBuffer(buffer **b)
{
	if(*b == NULL)
	{
		return;
	}	

	if((*b) -> str != NULL)
	{
		free((*b) -> str);
	}

	free(*b);
	*b = NULL;
}

