#Makefile for the 8080 Emulator

#Compiler Variable
CC = gcc

#Compiler Flags
#-g - generate debuggable executable code
#-Wall - generate most compiler warnings
FLAGS = -g -Wall -Wpadded

#Source Files
SRC = emulator_8080.o

#Executable File
TARGET = emu


all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(FLAGS) $(SRC) -o $(TARGET) 

8080_emulator.o: emulator_8080.c
	gcc -g -Wall -c emulator_8080.c

compile_emulator: emulator_8080.c
	gcc -Wall -c emulator_8080.c 2> error.txt

assembler: assembler_8080.c
	gcc -Wall -g assembler_8080.c -o assembler

compile_assembler: assembler_8080.c
	gcc -Wall -c assembler_8080.c 2> error.txt

clean:
	rm -rf *o


