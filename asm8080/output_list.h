/********************************************************************************
 * 8080 Output List Object for the 8080 Assembler				*
 * Pramuka Perera								*
 * 26 July, 2017								*
 * 8080's instruction set:							*
 * Used by the assembler to store the final output values prior to actual output*
 ********************************************************************************/

/*
typedef struct output_node
{
	int opcode;
	char *operand;
	struct output_node *next;
}
ouput;


typedef struct output_head_node
{
	int opcode;
	char *operand;
	output *next;
	output *last;
}
output_head;
*/

typedef struct output_node
{
	int opcode;
	char *operand;
	struct output_node *next;
	struct output_node *last;
}
output;

void AddOutputNode(int new_opcode, char *new_operand, output **head)
{
	output *new_node;

	/*
	if(*head == NULL)
	{
		if((*head = malloc(sizeof(output_head))) == NULL)
		{
			printf("Failed to allocate output list.\n");
			exit(0);
		}
		
		*head -> opcode = new_opcode;
		*head -> operand = new_operand;
		*head -> next = NULL;
		*head -> last = *head;	

		return;
	}
	*/

	if((new_node = malloc(sizeof(output))) == NULL)
	{
		printf("Failed to allocate output node.\n");
		exit(0);
	}

	new_node -> opcode = new_opcode;
	new_node -> next = NULL;
	
	new_node -> operand = NULL;
	//allocate space for new_operand including null terminator
	if((new_node -> operand = malloc((strlen(new_operand) + 1) * sizeof(char))) == NULL)
	{
		printf("Failed to allocate string.\n");
		exit(0);
	}
	strcpy(new_node -> operand, new_operand);
	
	if(*head == NULL)
	{
		new_node -> last = *head;
		*head = new_node;
		
		return;
	}	

	((*head) -> last) -> next = new_node;
	(*head) -> last = new_node; 
}

//void ReplaceLabelWithValue(

void FreeOutputList(output **head)
{
	output 	*o,
		*temporary_ptr;

	if(*head == NULL)
	{
		return;
	}

	o  = (*head) -> next;

	//since we already have the next output, we can free this one
	free(*head);
	*head = NULL;

	while(o != NULL)
	{
		temporary_ptr = o -> next;
		free(o -> operand);
		free(o);
		o = temporary_ptr;
	}
}
