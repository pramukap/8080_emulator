#Makefile for the 8080 Emulator

SHELL = /bin/sh

#EMU_SRC = ~/src/8080_emulator/emu8080/
#ASM_SRC = ~/src/8080_emulator/asm8080/

all:emu asm
	
emu:
	make -C ~/src/8080_emulator/emu8080
asm:
	make -C ~/src/8080_emulator/asm8080
