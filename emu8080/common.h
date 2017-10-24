/************************************************************************
 * 8080 Instruction Set							*
 * Pramuka Perera							*
 * August 5, 2017							*
 * Array of the 8080 instruction set, including a function pointer	*
 * to the function that emulates the function				* 
 ************************************************************************/

#define INSTRUCTION_SET_SIZE	256

#define MASK0			0x01
#define MASK1			0x02
#define MASK2			0x04
#define MASK3			0x08
#define MASK4			0x10
#define MASK5			0x20
#define MASK6			0x40
#define MASK7			0x70

#define CY			0x01
#define AC			0x02
#define S			0x04
#define Z			0x08
#define EP			0x10

#define C			0
#define B			1
#define E			2
#define D			3
#define L			4
#define H			5
#define A			6
#define STATUS			7
//#define Z			8
//#define W			9

#define B_PAIR			0
#define D_PAIR			2
#define H_PAIR			4
#define PSW			6

#define HARD_DISK_SIZE		0xffff		//space in non-volatile memory in bytes
#define ADDRESSED_SPACE_SIZE	0xffff	
#define MEMORY_START_ADDRESS	0x0000		//volatile memory (0x0000 to 0x2fff) (3 kB)
#define MEMORY_SIZE		0x3000
//#define IO_START_ADDRESS	0x3000		//memory-mapped io (0x3000 to 0x3fff)
//#define IO_SIZE		0x1000
#define VIDEO_MEM_START_ADDRESS	0x4000		//graphics memory (0x4000 to 0x7fff) (4 kB)	
#define VIDEO_MEM_SIZE		0x4000
#define PORTS			256

#define BYTE			8
#define ALL			0b00011111
#define NONE			0b00000000
#define ALL_EXCEPT_CARRY	0b00011110
#define CARRY			0b00000001

#define NV_MEM_CTRL_REG		0x3ffc
#define NV_MEM_DATA_REG		0x3ffd
#define NV_MEM_ADDR_LOW		0x3ffe
#define NV_MEM_ADDR_HIGH	0x3fff	

#define KB_CTRL_REG		0x3ff9
#define KB_DATA_REG		0x3ffa

#define INTERRUPT_ENABLE	0x01
#define RDY			0x02
#define READ_REQUEST		0x04
#define WRITE_REQUEST		0x08
#define DONE			0x10

#define CLOCK_RATE		2500000		//Hz (2.5 MHz = 4 us) 

//HARDWARE---
extern uint8_t 	*address_space,
		*hard_disk,
		*memory,
		*video_memory,
		*io; 

extern uint8_t register_file[10];

/*
 * General Purpose Registers
 * x86 stores data using little endian format.
 *
 * These registers are accessed as register pairs (where B, D, and H hold the higher order byte) 
 * using  a uint16_t pointer in various instructions.
 *
 * As a result of these two facts, the registers are described in an order where the low order 
 * register is assigned a lower address than it's higher order counterpart.
 */
extern uint8_t *const c;
extern uint8_t *const b;
extern uint8_t *const e;
extern uint8_t *const d;
extern uint8_t *const l;
extern uint8_t *const h;

//Accumulator
extern uint8_t *const a;

/*
 * Status Byte
 * FLAGS - Carry | Aux Carry | Sign | Zero | Even Parity
 * BIT   - 0     | 1         | 2    | 3    | 4 
 */
extern uint8_t *const status;

//Registers Z and W can only be used for instruction execution
//These registers are not directly accessible to the programmer
//static uint8_t *const z = register_file + Z;
//static uint8_t *const w = register_file + W;

//Register Pairs
//B+C
extern uint16_t *const b_pair;
//D+E
extern uint16_t *const d_pair;
//H+L
extern uint16_t *const h_pair;
//A+status
extern uint16_t *const psw;

/*
 * The pc and sp contain the emulated address and not the host address.
 * The emulated address is in fact an index for the emulated memory,
 * which is a dynamically-allocated array.
 */
extern uint16_t pc;
extern uint16_t sp;

//Instruction Register
extern uint8_t instruction_register;

//Processor Time
extern uint32_t time;
//---

//I/O---
//Control I/O Signals of 8080
//SIGNALS - WR'(0) | DBIN(O) | INTE(O) | INT(I) | HOLD ACK(O) | HOLD(I) | WAIT (0) | READY(I) | SYNC(O) | RESET(I) 
//BIT     - 0      | 1       | 2       | 3      | 4           | 5       | 6        | 7        | 8       | 9
extern uint16_t control;
extern uint8_t interrupt_enable;
extern uint8_t halt_enable;
extern uint8_t interrupt_request;
//---

//USER INTERFACE---
//Signals presented on the 8800 front panel
//SIGNALS - INTE | PROT | MEMR | INP | M1 | OUT | HLTA | STACK | WO'  | INT(A) | WAIT | HLDA | RESET
//BIT     - 0    | 1    | 2    | 3   | 4  | 5   | 6    | 7     | 8    | 9      | 10   | 11   | 12
extern uint16_t indicator;
//---

//8080 INSTRUCTION SET---
//Stores data relevant to instruction; accessed by instruction-emulating function
typedef struct instruction_data 
{
	uint8_t *const register_1;	//source register or only register mentioned in instruction
	uint8_t *const register_2;	//destination register
	uint16_t *const register_pair; 	//source or destination register pair
	const char name[12];		//Instruction mnemonic		
	const uint8_t size;		//size of instruction in bytes
	const uint8_t flags;		//flags triggered by instruction
	const uint8_t duration;		//number of clock cycles instruction takes (instructions marked that have 0xFF have (11 or 17) cc or (5 or 11) cc
} data; 

//Function that emulates instruction
typedef void (*instruction)(data *input); 

extern data instruction_set_data[INSTRUCTION_SET_SIZE];
extern instruction instruction_set[INSTRUCTION_SET_SIZE];


//Enumerated type to detect which device sent an interrupt request
typedef enum interrupt_request_device
{
	NO_INTERRUPT	= 0x00, 
	STORAGE_WRITE 	= 0xd7,
	STORAGE_READ	= 0xdf,
	KEYBOARD	= 0xe7,
	DISPLAY 	= 0xef
} interrupt_device;

extern interrupt_device interrupt_vector; 

typedef enum io_operation_state
{
	READY,
	READING,
	WRITING,
	INTERRUPT,
	OP_COMPLETE
} io_state;
