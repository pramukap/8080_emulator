/********************************************************
 * VT100 Terminal					*
 * Pramuka Perera					*
 * August 16, 2017					*
 * VT100 I/O Terminal for 8080 Computer			*
 ********************************************************/

#ifndef INCLUDE
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

#include <limits.h>
#include <curses.h>

//#define MAX_ROWS	1000
//#define MAX_COLS	500

#define KB_READ_RATE		20	//Hz (80 ms)
#define KB_READ_PERIOD		(CLOCK_RATE / KB_READ_RATE) //Clock Cycles

int 	max_columns, 
	max_rows,
	col,
	row;

WINDOW *monitor;

void StartMonitor()
{
	monitor = initscr();
	cbreak();
	noecho();
	nodelay(monitor, FALSE);
	getmaxyx(monitor, max_rows, max_columns);
	clear();
	refresh();
}

void PrintMachineState()
{
	char 	data[2],
		address[4];	

	clear();
	move(0,0);

	addstr("General Purpose Registers: ");

	sprintf(data, "%02x", b[0]);
	addstr("B -> ");
	addstr(data);
	addstr(" ");

	sprintf(data, "%02x", c[0]);
	addstr("C -> ");
	addstr(data);
	addstr(" ");
	
	sprintf(data, "%02x", d[0]);
	addstr("D -> ");
	addstr(data);
	addstr(" ");

	sprintf(data, "%02x", e[0]);
	addstr("E -> ");
	addstr(data);
	addstr(" ");

	sprintf(data, "%02x", h[0]);
	addstr("H -> ");
	addstr(data);
	addstr(" ");

	sprintf(data, "%02x", l[0]);
	addstr("L -> ");
	addstr(data);
	addstr(" ");

	addstr("\n");

	addstr("State Registers: ");

	sprintf(address, "%02x", pc);
	addstr("PC -> ");
	addstr(address);
	addstr(" ");
	
	sprintf(address, "%02x", sp);
	addstr("SP -> ");
	addstr(address);
	addstr(" ");

	sprintf(address, "%02x", status[0]);
	addstr("FLAGS -> ");
	addstr(address);
	addstr(" ");

	refresh();	
}

/*
 * Memory - Mapped Keyboard Register
 * 0x3ff9 - Keyboard Control Register
 * BITS:	2			1		0
 * VALUE:	Read-Request Flag	Ready Flag	Interrupt-Enable
 * 0x3ffa - Keyboard Data Register
 */
 
void ReadKeyboardInput()
{
	static int kb_op_completion_time = INT_MAX;

	if(memory[KB_CTRL_REG] & READY)
	{
		if(memory[KB_CTRL_REG] & READ_REQUEST)
		{
			kb_op_completion_time = time + KB_READ_PERIOD;   
		
			memory[KB_CTRL_REG] &= ~READY;	
		}	
	}	
	else if((memory[KB_CTRL_REG] & READY) == 0 && time >= kb_op_completion_time)
	{
		//complete operation only if keyboard provides an actual key value
		if((memory[KB_DATA_REG] = getchar()) != ERR)
		{
			if(memory[KB_CTRL_REG] & INTERRUPT_ENABLE)
			{
				interrupt_request = 1;
			}
			
			memory[KB_CTRL_REG] |= READY;
			kb_op_completion_time = INT_MAX;
		}	
	} 
}



void StopMonitor()
{
	endwin();
}

