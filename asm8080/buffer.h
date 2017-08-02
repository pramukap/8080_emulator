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

	#define INCLUDE
#endif

typedef struct str_buffer
{
	char *str;
	int index;
	int length;
}
buffer;

buffer *NewBuffer(void)
{
	buffer *b;

	if((b = malloc(sizeof(buffer))) == NULL)
	{
		printf("Insufficient memory to allocate a buffer.\n");
		exit(0);
	}

	if(((b -> str) = malloc(sizeof(char))) == NULL)
	{
		printf("Insufficient memory to allocate a buffer string.\n");
		free(b);
		exit(0);
	}

	b -> index = 0;
	b -> length = 1;

	return b;
}

void AddCharToBuffer(buffer *b,  char c)
{
	buffer *temporary_ptr = b;

	if((b -> str = realloc(b -> str, ((b -> length) + 1) * sizeof(char))) == NULL)
	{
		printf("Insufficient memory to allocate a buffer.\n");
		//free allocated memory
		free(temporary_ptr);
		exit(0);
	}

	(b -> str)[b -> index] = c;	

	(b -> length)++;
	(b -> index)++;
}

//If c is an uppercase letter, conver it to a lowercase letter
char ToLower(char c)
{
	if(c >= 'A' && c <= 'Z')
	{
		c += 0x20;
	}
	
	return c;
}

void ShiftBufferContentsLeft(buffer *b)
{
	int i = 1;

	while(i < (b -> length))
	{
		(b -> str)[i-1] = (b -> str)[i];
		
		i++;
	}

	(b -> str)[(b -> length) - 1] = '\0';
	(b -> length)--;
}

void FreeBuffer(buffer **b)
{
	if(*b == NULL)
	{
		return;
	}	

	free(*b);
	*b = NULL;
}

