/********************************************************************************
 * 8080 Pseudo-Instruction Set Object for the 8080 Assembler			*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * 8080's assembler level instructions:						*
 * Pseudo-instructions help the assembler put the program into the computer	*
 ********************************************************************************/

typdef struct pseudo_instruction
{
	char *mnemonic;
}
psuedo;

char pseudo_instruction_set[8] = 
{
	"org",
	"equ",
	"set",
	"end",
	"db",
	"dw",
	"ds",
	"if",
	"endif",
	"macro",
	"endm"
};

enum pseudo_instruction
{
	ORG,
	EQU,
	SET,
	END,
	DB,
	DW,
	DS,
	
}

void Org(char *line)
{
	
}

void FindPseudoInstruction(char *line)
{
	int 	i,
		result = -1;
	char 	*pseudo;

	for(i = 0; i < 9 && result != 0; i++)
	{
		if(i == 9)
		{
			//no pseudo instruction found
		}

		pseudo = pseudo_instruction_set[i];
		result = strncmp(pseudo, line, strlen(pseudo));
	}

	switch(pseudo)
	{
		
	}
}
