#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct instruction_data
{
	uint16_t a;
	uint8_t b;
}
data;

uint8_t register_file[10];

uint8_t *reg1 = register_file + 0;
uint16_t *reg1_pair = (uint16_t *)(register_file + 0);

typedef int (*instruction)(uint8_t *a, uint16_t *b);

int test(uint8_t *c, uint16_t *d)
{
	printf("Works\n");
	return 2;	
}

int test2(uint8_t *c, uint16_t *d)
{
	printf("Works\n");
	return 3;	
}


void test3()
{
	char buffer[3];
	char* ptr = NULL;
	uint8_t num = 0;

	printf("Input instruction: \n");

	if(!(scanf("%2s", buffer) == 1) || buffer[3] != 0)
	{
		printf("Input Too Large");
		exit(0);
	}

	printf("%s\n", buffer);

	if(!((buffer[1] >= 48 && buffer[1] <= 57) || (buffer[1] >= 65 && buffer[1] <= 70) || (buffer[1] >= 97 && buffer[1] <= 102))) 
	{
		printf("Improper value in input\n");
		exit(0);
	}
	num = (uint8_t)strtol(buffer, &ptr, 16);
	printf("%i\n", num);
}

instruction instruction_set[2] = {test, test2};

int main()
{	
	int i = 0;
	int c = 0x80;
	
	i = instruction_set[1](reg1, reg1_pair);
	printf("%i\n", i);

	printf("%i\n", c>>7);

	test3();
}

