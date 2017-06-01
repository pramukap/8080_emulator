#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct instruction 
{	
	//Bits			[15:12] [11:10]			  [9:5]         [4:0]
	//Data			        |SIZE(BYTES EXCL. OPCODE) |DURATION(CC) |FLAGS(Even Parity, Zero, Sign, Aux Carry, Carry)
	uint16_t data;
	uint32_t (*instFunc)(void); //Function that emulates instruction; returns duration of instruction
	char* name;		//Instruction mnemonic		
} inst;



void main(void){

uint8_t gpr[10];

uint8_t* B = gpr+0;
uint8_t* C = gpr+1;
uint8_t* D = gpr+2;
uint8_t* E = gpr+3;
uint8_t* H = gpr+4;
uint8_t* L = gpr+5;
//Accumulator
uint8_t* A = gpr+6;
//FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
//BIT   - 0     | 1         | 2    | 3    | 4 
uint8_t* status = gpr+7;
//Registers Z and W can only be used for instruction execution
//These registers are not directly accessible to the programmer
uint8_t* Z = gpr+8;
uint8_t* W = gpr+9;

//B+C
uint16_t* Bp = (uint16_t*)(gpr+0);
//D+E
uint16_t* Dp = (uint16_t*)(gpr+2);
//H+L
uint16_t* Hp = (uint16_t*)(gpr+4);
//A+status
uint16_t* PSW = (uint16_t*)(gpr+6);

uint8_t* pc;
uint8_t* sp;

//Control I/O Signals of 8080
//SIGNALS - WR'(0) | DBIN(O) | INTE(O) | INT(I) | HOLD ACK(O) | HOLD(I) | WAIT (0) | READY(I) | SYNC(O) | RESET(I) 
//BIT     - 0      | 1       | 2       | 3      | 4           | 5       | 6        | 7        | 8       | 9
uint16_t control;

//Signals presented on the 8800 front panel
//SIGNALS - INTE | PROT | MEMR | INP | M1 | OUT | HLTA | STACK | WO'  | INT(A) | WAIT | HLDA | RESET
//BIT     - 0    | 1    | 2    | 3   | 4  | 5   | 6    | 7     | 8    | 9      | 10   | 11   | 12
uint16_t indicator;

uint8_t inst_reg;
uint32_t time = 0;

//machine cycle operations
//FETCH - reads next program instruction, and stores it in the instruction register; then increments the pc
void fetch(uint8_t* pc){
	inst_reg = *pc;
	pc = pc+1;
}

//MEMORY READ - reads data/address at adr to the register or register pair indicated by the select (0 = r, 1 = rp)   
void memR(uint8_t* adr, uint8_t* dr, uint8_t select){
	if(select == 1)
	{
		dr[1] = adr[1];
		return;
	}

	dr[0] = *adr;
}

//MEMORY WRITE - writes data in source register or source register pair (sr) as indicated by the select (0 = r, 1 = rp) to address (adr)  
void memW(uint8_t* adr, uint8_t* sr, uint8_t select){
	if(select == 1)
	{
		adr[1] = sr[1];
		return;
	}

	adr[0] = sr[0];
}

//STACK READ - reads data/address stored on stack to the register or register pair indicated by the select (0 = r, 1 = rp)
void stackR(uint8_t* sp, uint8_t* dr, uint8_t select){
	if(select == 1)
	{
		dr[1] = sp[1];
		dr[0] = sp[0];
		sp += 2;
		return;
	}

	dr[0] = sp[0];
	sp += 1;
}

//STACK WRITE - writes data stored in source register or register pair (sr) as indiacted by select (0 = r, 1 = rp) to stack
void stackW(uint8_t* sp, uint8_t* sr, uint8_t select){
	if(select == 1)
	{
		sp -= 2;
		sp[1] = sr[1];
		sp[0] = sr[0];
		return;
	}

	sp[0] = sr[0];
}	

//INPUT READ:w

//8080 Instruction Set
inst inst_set[246] = {		//	mnemonic      	SIZE,DURATION,FLAGS (EP, Z, S, AC, C)
{0x0080,,"NOP"} 		//0	NOP 		S0,D4,F00000	0000 0000 1000 0000
{0x0940,,"LXI B, D16"}		//1	LXI B		S2,D10,F00000   0000 1001 0100 0000
{0x00E0,,"STAX B"}		//2	STAX B		S0,D7,F00000    0000 0000 1110 0000
{0x00A0,,"INX B"}		//3	INX B		S0,D5,F00000	0000 0000 1010 0000
{0x00BF,,"INR B"}		//4	INR B		S0,D5,F11110	0000 0000 1011 1111
{0x00BF,,"DCR B"}		//5	DCRB		S0,D5,F11110		"
{0x04E0,,"MVI B D8"}		//6	MVI B D8	S1,D7,F00000	0000 0100 1110 0000
{0x0081,,"RLC"}			//7	RLC		S0,D4,F00001	0000 0000 1000 0001
				//8
{0x0141,,"DAD B"}		//9	DAD B		S0,D10,F00001	0000 0001 0100 0001
{0x00E0,,"LDAX B"}		//10	LDAX B		S0,D7,F00000	0000 0000 1110 0000
{0x00A0,,"DCX B"}		//11	DCX B		S0,D5,F00000	0000 0000 1010 0000
{0x00DE,,"INR C"}		//12	INR C		S0,D5,F11110	0000 0000 1011 1110
};

			
}
	
