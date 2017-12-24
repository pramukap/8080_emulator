#Makefile for the 8080 Emulator

SHELL = /bin/sh

#EMU_SRC = ~/src/8080_emulator/emu8080/
#ASM_SRC = ~/src/8080_emulator/asm8080/

all:emu asm
	
emu:
	(cd emu8080; $(MAKE))

asm:
	(cd asm8080; $(MAKE))

clean:
	rm emu; rm asm
