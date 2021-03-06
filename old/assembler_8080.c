#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define	NEWLINE	0x0a

typedef struct instruction_pair
{
	char* mnemonic;
	uint8_t opcode;
}
pair;


pair instruction_set[256] =
{
	/*00*/	{"aci", 0xce},
		{"adc a,", 0x8f},
	/*02*/	{"adc b,", 0x88},
		{"adc c,", 0x89},
	/*04*/	{"adc d,", 0x8a},
		{"adc e,", 0x8b},
	/*06*/	{"adc h,", 0x8c},
		{"adc l,", 0x8d},
	/*08*/	{"adc m,", 0x8e},
		{"add a,", 0x87},
	/*0a*/	{"add b,", 0x80},
		{"add c,", 0x81},
	/*0c*/	{"add d,", 0x82},
		{"add e,", 0x83},
	/*0e*/	{"add h,", 0x84},
		{"add l,", 0x85},
	/*10*/	{"add m,", 0x86},
		{"adi", 0xc6},
	/*12*/	{"ana a", 0xa7},
		{"ana b", 0xa0},
	/*14*/	{"ana c", 0xa1},
		{"ana d", 0xa2},
	/*16*/	{"ana e", 0xa3},
		{"ana h", 0xa4},
	/*18*/	{"ana l", 0xa5},
		{"ana m", 0xa6},
	/*1a*/	{"ani", 0xe6},
		{"call", 0xcd},
	/*1c*/	{"cc", 0xdc},
		{"cm", 0xfc},
	/*1e*/	{"cma", 0x2f},
		{"cmc", 0x3f},
	/*20*/	{"cmp a,", 0xbf},
		{"cmp b,", 0xb8},
	/*22*/	{"cmp c,", 0xb9},
		{"cmp d,", 0xba},
	/*24*/	{"cmp e,", 0xbb},
		{"cmp h,", 0xbc},
	/*26*/	{"cmp l,", 0xbd},
		{"cmp m,", 0xbe},
	/*28*/	{"cnc", 0xd4},
		{"cnz", 0xc4},
	/*2a*/	{"cp", 0xf4},
		{"cpe", 0xec},
	/*2c*/	{"cpi", 0xfe},
		{"cpo", 0xe4},
	/*2e*/	{"cz", 0xcc},
		{"daa", 0x27},
	/*30*/	{"dad b", 0x09},
		{"dad d", 0x19},
	/*32*/	{"dad h", 0x29},
		{"dad sp", 0x39},
	/*34*/	{"dcr a", 0x3d},
		{"dcr b", 0x05},
	/*36*/	{"dcr c", 0x0d},
		{"dcr d", 0x15},
	/*38*/	{"dcr e", 0x1d},
		{"dcr h", 0x25},
	/*3a*/	{"dcr l", 0x2d},
		{"dcr m", 0x35},
	/*3c*/	{"dcx b", 0x0b},
		{"dcx d", 0x1b},
	/*3e*/	{"dcx h", 0x2b},
		{"dcx sp", 0x3b},
	/*40*/	{"di", 0xf3},
		{"ei", 0xfb},
	/*42*/	{"hlt", 0x76},
		{"in", 0xdb},
	/*44*/	{"inr a", 0x3c},
		{"inr b", 0x04},
	/*46*/	{"inr c", 0x0c},
		{"inr d", 0x14},
	/*48*/	{"inr e", 0x1c},
		{"inr h", 0x24},
	/*4a*/	{"inr l", 0x2c},
		{"inr m", 0x34},
	/*4c*/	{"inx b", 0x03},
		{"inx d", 0x13},
	/*4e*/	{"inx h", 0x23},
		{"inx sp", 0x33},
	/*50*/	{"jc", 0xda},
		{"jm", 0xfa},
	/*52*/	{"jmp", 0xc3},
		{"jnc", 0xd2},
	/*54*/	{"jnz", 0xc2},
		{"jp", 0xf2},
	/*56*/	{"jpe", 0xea},
		{"jpo", 0xe2},
	/*58*/	{"jz", 0xca},
		{"lda", 0x3a},
	/*5a*/	{"ldax b", 0x0a},
		{"ldax d", 0x1a},
	/*5c*/	{"lhld", 0x2a},
		{"lxi b,", 0x01},
	/*5e*/	{"lxi d,", 0x11},
		{"lxi h,", 0x21},
	/*60*/	{"lxi sp,", 0x31},
		{"mov a,a", 0x7f},
	/*62*/	{"mov a,b", 0x78},
		{"mov a,c", 0x79},
	/*64*/	{"mov a,d", 0x7a},
		{"mov a,e", 0x7b},
	/*66*/	{"mov a,h", 0x7c},
		{"mov a,l", 0x7d},
	/*68*/	{"mov a,m", 0x7e},
		{"mov b,a", 0x47},
	/*6a*/	{"mov b,b", 0x40},
		{"mov b,c", 0x41},
	/*6c*/	{"mov b,d", 0x42},
		{"mov b,e", 0x43},
	/*6e*/	{"mov b,h", 0x44},
		{"mov b,l", 0x45},
	/*70*/	{"mov b,m", 0x46},
		{"mov c,a", 0x4f},
	/*72*/	{"mov c,b", 0x48},
		{"mov c,c", 0x49},
	/*74*/	{"mov c,d", 0x4a},
		{"mov c,e", 0x4b},
	/*76*/	{"mov c,h", 0x4c},
		{"mov c,l", 0x4d},
	/*78*/	{"mov c,m", 0x4e},
		{"mov d,a", 0x57},
	/*7a*/	{"mov d,b", 0x50},
		{"mov d,c", 0x51},
	/*7c*/	{"mov d,d", 0x52},
		{"mov d,e", 0x53},
	/*7e*/	{"mov d,h", 0x54},
		{"mov d,l", 0x55},
	/*80*/	{"mov d,m", 0x56},
		{"mov e,a", 0x5f},
	/*82*/	{"mov e,b", 0x58},
		{"mov e,c", 0x59},
	/*84*/	{"mov e,d", 0x5a},
		{"mov e,e", 0x5b},
	/*86*/	{"mov e,h", 0x5c},
		{"mov e,l", 0x5d},
	/*88*/	{"mov e,m", 0x5e},
		{"mov h,a", 0x67},
	/*8a*/	{"mov h,b", 0x60},
		{"mov h,c", 0x61},
	/*8c*/	{"mov h,d", 0x62},
		{"mov h,e", 0x63},
	/*8e*/	{"mov h,h", 0x64},
		{"mov h,l", 0x65},
	/*90*/	{"mov h,m", 0x66},
		{"mov l,a", 0x6f},
	/*92*/	{"mov l,b", 0x68},
		{"mov l,c", 0x69},
	/*94*/	{"mov l,d", 0x6a},
		{"mov l,e", 0x6b},
	/*96*/	{"mov l,h", 0x6c},
		{"mov l,l", 0x6d},
	/*98*/	{"mov l,m", 0x6e},
		{"mov m,a", 0x77},
	/*9a*/	{"mov m,b", 0x70},
		{"mov m,c", 0x71},
	/*9c*/	{"mov m,d", 0x72},
		{"mov m,e", 0x73},
	/*9e*/	{"mov m,h", 0x74},
		{"mov m,l", 0x75},
	/*a0*/	{"mvi a,", 0x3e},
		{"mvi b,", 0x06},
	/*a2*/	{"mvi c,", 0x0e},
		{"mvi d,", 0x16},
	/*a4*/	{"mvi e,", 0x1e},
		{"mvi h,", 0x26},
	/*a6*/	{"mvi l,", 0x2e},
		{"mvi m,", 0x36},
	/*a8*/	{"nop", 0x00},
		{"ora a", 0xb7},
	/*aa*/	{"ora b", 0xb0},
		{"ora c", 0xb1},
	/*ac*/	{"ora d", 0xb2},
		{"ora e", 0xb3},
	/*ae*/	{"ora h", 0xb4},
		{"ora l", 0xb5},
	/*b0*/	{"ora m", 0xb6},
		{"ori", 0xf6},
	/*b2*/	{"out", 0xd3},
		{"pchl", 0xe9},
	/*b4*/	{"pop b", 0xc1},
		{"pop d", 0xd1},
	/*b6*/	{"pop h", 0xe1},
		{"pop psw", 0xf1},
	/*b8*/	{"push b", 0xc5},
		{"push d", 0xd5},
	/*ba*/	{"push h", 0xe5},
		{"push psw", 0xf5},
	/*bc*/	{"ral", 0x17},
		{"rar", 0x1f},
	/*be*/	{"rc", 0xd8},
		{"ret", 0xc9},
	/*c0*/	{"rlc", 0x07},
		{"rm", 0xf8},
	/*c2*/	{"rnc", 0xd0},
		{"rnz", 0xc0},
	/*c4*/	{"rp", 0xf0},
		{"rpe", 0xe8},
	/*c6*/	{"rpo", 0xe0},
		{"rrc", 0x0f},
	/*c8*/	{"rst 0", 0xc7},
		{"rst 1", 0xcf},
	/*ca*/	{"rst 2", 0xd7},
		{"rst 3", 0xdf},
	/*cc*/	{"rst 4", 0xe7},
		{"rst 5", 0xef},
	/*ce*/	{"rst 6", 0xf7},
		{"rst 7", 0xff},
	/*d0*/	{"rz", 0xc8},
		{"sbb a", 0x9f},
	/*d2*/	{"sbb b", 0x98},
		{"sbb c", 0x9a},
	/*d4*/	{"sbb d", 0x9b},
		{"sbb e", 0x9c},
	/*d6*/	{"sbb h", 0x9d},
		{"sbb l", 0x9e},
	/*d8*/	{"sbb m", 0x9f},
		{"sbi", 0xde},
	/*da*/	{"shld", 0x22},
		{"sphl", 0xf9},
	/*dc*/	{"sta", 0x32},
		{"stax b", 0x02},
	/*de*/	{"stax d", 0x12},
		{"stc", 0x37},
	/*e0*/	{"sub a", 0x97},
		{"sub b", 0x90},
	/*e2*/	{"sub c", 0x91},
		{"sub d", 0x92},
	/*e4*/	{"sub e", 0x93},
		{"sub h", 0x94},
	/*e6*/	{"sub l", 0x95},
		{"sub m", 0x96},
	/*e8*/	{"sui", 0xd6},
		{"xchg", 0xeb},
	/*ea*/	{"xra a", 0xaf},
		{"xra b", 0xa8},
	/*ec*/	{"xra c", 0xa9},
		{"xra d", 0xaa},
	/*ee*/	{"xra e", 0xab},
		{"xra h", 0xac},
	/*f0*/	{"xra l", 0xad},
		{"xra m", 0xae},
	/*f2*/	{"xri", 0xee},
		{"xthl", 0xe3}
};

typedef struct list_node
{
	char label[5];
	uint16_t address;
	struct list_node *next_node;
	struct list_node *last_node;
}
node;

void AddLabelNode(char *new_label, uint16_t new_address, node ** head)
{
	node 	*new_node;

	if((new_node = malloc(sizeof(node))) == NULL)
	{
		printf("Couldn't allocate space to save a label node.");
		exit(0);
	}

	strncpy(new_node -> label, new_label, sizeof(new_node -> label));	
	new_node -> address = new_address;
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

uint32_t FindLabelValue(char *label_of_address, node *head)
{
	node *current_node = head;
	
	while(current_node)
	{
		if(strcmp(current_node -> label, label_of_address) == 0)
		{
			return (current_node -> address);
		}

		current_node = current_node -> next_node; 
	}

	return 0x10000;	
}

void FreeLabelList(node *head)
{
	node *current_node = head;
	node *next_node;

	while(current_node)
	{
		next_node = current_node -> next_node;
		free(current_node);
		current_node = next_node;
	}
}

typedef struct line_token
{
	char *token;
	int token_size;
}
token;



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
			lines[line_index].token = buffer;
			lines[line_index].token_size = buffer_size;
			
			printf("Line Token #%i: %s\n", line_index, lines[line_index].token);

			//add new token struct
			temporary_ptr = lines;
			if((lines = realloc(lines, (num_line_tokens + 1) * sizeof(token))) == NULL)
			{
				printf("No more memory to store program. Please try again.\n");
				free(temporary_ptr);
				exit(0);
			}
			lines[line_index + 1].token = NULL; 

			line_index += 1;
			num_line_tokens += 1;			

			//reset buffer
			buffer = malloc(sizeof(char));
			buffer_index = 0;
			buffer_size = 1;
	
			//if ; then go to new line	
			while(c == ';' && assembly_code[code_index] != NEWLINE)
{
	code_index += 1;
} 
	
			code_index++;
			continue;
		}  		

		buffer[buffer_index] = c;	
		buffer_index++;
		buffer_size++;

		code_index++;
	}

	//process operands

	//free memory
	
	return 1;
}

/*
if(c == ' ' || c == ',' && buffer_size > 0)
{
	program[program_index].token = buffer;
	program[program_index].token = buffer_size;
	
	buffer = malloc(sizeof(char));

	buffer_size = 0;
	buffer_index = 0;
	
	program = realloc(program, (num_tokens + 1) * sizeof(token));
	
	num_tokens += 1;
}
*/
