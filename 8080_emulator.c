#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static typedef struct instruction 
{
	//Bits			[15:12] [11:10]			  [9:5]         [4:0]
	//Data			        |SIZE(BYTES EXCL. OPCODE) |DURATION(CC) |FLAGS(Even Parity, Zero, Sign, Aux Carry, Carry)
	//uint16_t data;
	uint8_t size;
	uint8_t flags;
	uint8_t duration;
	uint32_t (*instFunc)(void); //Function that emulates instruction; returns duration of instruction
	char *name;		//Instruction mnemonic		
} inst;

static uint8_t const *mem = (uint8_t)malloc(4000*sizeof(uint8_t));

static uint8_t gpr[10];

static uint8_t const *B = gpr+0;
static uint8_t const *C = gpr+1;
static uint8_t const *D = gpr+2;
static uint8_t const *E = gpr+3;
static uint8_t const *H = gpr+4;
static uint8_t const *L = gpr+5;
//Accumulator
static uint8_t const *A = gpr+6;
//FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
//BIT   - 0     | 1         | 2    | 3    | 4 
static uint8_t const *status = gpr+7;
//Registers Z and W can only be used for instruction execution
//These registers are not directly accessible to the programmer
static uint8_t const *Z = gpr+8;
static uint8_t const *W = gpr+9;

//B+C
static uint16_t const *Bp = (uint16_t*)(gpr+0);
//D+E
static uint16_t const *Dp = (uint16_t*)(gpr+2);
//H+L
static uint16_t const *Hp = (uint16_t*)(gpr+4);
//A+status
static uint16_t const *PSW = (uint16_t*)(gpr+6);

//use offset method so that if I read the value of the pc or sp, I get the emulated address and not the actual address of the host computer
static uint8_t pc;
static uint8_t sp;

//Control I/O Signals of 8080
//SIGNALS - WR'(0) | DBIN(O) | INTE(O) | INT(I) | HOLD ACK(O) | HOLD(I) | WAIT (0) | READY(I) | SYNC(O) | RESET(I) 
//BIT     - 0      | 1       | 2       | 3      | 4           | 5       | 6        | 7        | 8       | 9
static uint16_t control;

//Signals presented on the 8800 front panel
//SIGNALS - INTE | PROT | MEMR | INP | M1 | OUT | HLTA | STACK | WO'  | INT(A) | WAIT | HLDA | RESET
//BIT     - 0    | 1    | 2    | 3   | 4  | 5   | 6    | 7     | 8    | 9      | 10   | 11   | 12
static uint16_t indicator;

static uint8_t inst_reg;
static uint32_t time = 0;

/*machine cycle operations
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
*/

//8080 Instruction Set -> size, flags, duration, function, name
static inst inst_set[246] = {		//	mnemonic      	SIZE,DURATION,FLAGS (EP, Z, S, AC, C)
/*
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
*/
{0x00000, }
};

void main(){
	while(1)
	{
		inst_reg = mem[pc]	
	}
	free(mem);			
}

//Instruction-Emulating Functions
//Data Transfer

//MOV REG -> Move data8 from source register to destination register
void movR(uint8_t *dr, uint8_t *sr){
	*dr = *sr;	
}

//MOV MEM -> Move data8 from source register to destination memory (addr stored in register pair H) 
//	  OR Move data8 from source memory (addr stored in rp H) to destination register
//	  -> src_mem = 0 -> src = register, src_mem = 1 -> src = (H,L)
void movM(uint8_t *dest, uint8_t *src, uint8_t src_mem){
	uint16_t addr;		

	if(src_mem)
	{
		addr = src[0]<<8 + src[1];
		dest[0] = mem[addr];
		return;
	}

	addr = dest[0]<<8 + dest[1];
	mem[addr] = src[0];
}

//MVI -> Moves immediate data8 to register or memory as selected by dest_mem (0 = register, 1 = memory)
void mvi(uint8_t *dest, uint8_t dest_mem){
	uint16_t addr;	

	if(dest_mem)
	{
		addr = dest[0]<<8 + dest[1];
		mem[addr] = mem[pc];
		pc += 1;
		return;	
	}

	dest[0] = mem[pc];
	pc += 1;
}

//LXI -> Loads data stored at location specified by immediate address (High Order Byte = byte 3, Low Order Byte = byte 2) to the specified register pair
void lxi(uint8_t *rp){
	uint8_t high_byte, low_byte;
	uint16_t addr;

	low_byte = mem[pc+0];
	high_byte = mem[pc+1];
	pc+=2;

	addr = high_byte<<8 + low_byte;
	rp[1] = mem[addr+0];
	rp[0] = mem[addr+1];
}

//LDA -> Loads data stored at location specified by immediate address to the accumulator register
//STA -> Stores data stored in accumulator to location specified by immediate address
//load_sel = 1, load data from addr to acc; load_sel = 0, store data from acc to addr
void stlda(uint8_t load_sel){
	uint8_t high_byte, low_byte;
	uint16_t addr;
	
	low_byte = mem[pc+0];
	high_byte = mem[pc+1];
	pc+=2;	

	addr = high_byte<<8 + low_byte;
	
	if(load_sel)
	{
		A[0] = mem[addr];
		return;
	}

	mem[addr] = A[0];
}

//LHLD -> Loads data stored at location specified by immediate address to HL register pair
//SHLD -> Stores data in HL register pair to memory location specified by immediate address
//load_sel = 1, load data from addr to HL; load_sel = 0, store data from HL to addr
void slhld(uint8_t load_sel){
	uint8_t high_byte, low_byte;
	uint16_t addr;
	
	low_byte = mem[pc+0];
	high_byte = mem[pc+1];
	pc+=2;
	
	addr = high_byte<<8 + low_byte;
	
	if(load_sel)
	{
		L[0] = mem[addr+0];
		H[0] = mem[addr+1];
		return;
	}

	mem[addr+0] = L[0];
	mem[addr+1] = H[0];
}

void stldax(uint16_t *rp, uint8_t load_sel){
	if(load_sel)
	{
		A[0] = mem[rp[0]];
		return;
	}

	mem[rp[0]] = A[0];
}

void xchg(){
	H[0] = H[0]^D[0];
	D[0] = D[0]^H[0];
	H[0] = H[0]^D[0];

	L[0] = L[0]^E[0];
	E[0] = E[0]^L[0];
	L[0] = L[0]^E[0];
}

//Arithmetic

//Logic

//Branch

//Stack, IO, Machine Control	
