/****************************************
 * 8080_Assembler v0.0.0		*
 * Pramuka Perera			*
 * July 23, 2017			*
 * Assembler for Intel 8080 Processor	*
 * Supports: Labels			*
 ****************************************/
#define	TAB	0x09
#define	NEWLINE	0x0a

#define MATCH	0x00

typedef enum pseudo_instruction_identifier
{
	NO_PSEUDO_FOUND,
	ORG,
	EQU,
	END	
}
pseudo_identifier;


typedef enum operand_type_identifier	
{
	NONE,
	D8,
	D16, 
	ADDR,
	ORG_ADDR
}
operand_type;

