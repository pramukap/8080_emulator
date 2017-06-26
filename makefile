#Makefile for the 8080 Emulator

#Compiler Variable
CC = gcc

#Compiler Flags
#-g - generate debuggable executable code
#-Wall - generate most compiler warnings
FLAGS = -g -Wall

#Source Files
SRC = 8080_emulator.o

#Executable File
TARGET = emu_make.exe


all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(FLAGS) $(SRC) -o $(TARGET)

8080_emulator.o: 8080_emulator.c
	gcc -g -Wall -c 8080_emulator.c

clean:
	rm -rf *o
