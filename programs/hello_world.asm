;CONSTANTS------
ADRL	EQU 01
ADRH	EQU 00
ADR_L	EQU 01
ADR_H	EQU 00
INTE	EQU 01
RDY	EQU 02
READ	EQU 04
WRITE	EQU 08
DONE	EQU 10
RESET	EQU 0F

CTRL	EQU 3ffc
DATA	EQU 3ffd
M_ADR_L	EQU 3ffe
M_ADR_H	EQU 3fff

;INSTRUCTIONS-----
MAIN:
	LXI H, HW
	CALL SFUNC
	HLT

;BITS	4	3	2	1	0
;FLAGS	DONE	WRITE	READ	RDY	INTE

;caller must save it's registers and store start of string address in HL
SFUNC:	
	MVI B, ADRL
	MVI C, ADRH		
	MVI D, 00

	;Store data to nv data register
LOOP:	MOV A, M 
	STA DATA
	MOV E, A

	;Store address to address register
NEXT:	MOV A, B
	STA M_ADR_L

	MOV A, C
	STA M_ADR_H

	;Modify control register to write to storage
	LDA CTRL
	ORI WRITE
	STA CTRL

	;Wait for storage to happen
	SWAIT: 	LDA CTRL
		ANI DONE	
		JZ SWAIT

	;Wait for reset to ready state
	RWAIT:	LDA CTRL
		MOV B, A
		ANI RDY
		JNZ FIN
 
		ANI RESET
		STA CTRL
		JMP RWAIT

	;If last char was zero, return from the function
	MOV A, E
	CMP D
	RZ

;DATA-------
;HELLO WORLD!\0
HW	DB 48
	DB 45
	DB 4C
	DB 4C
	DB 4F
	DB 20
	DB 57
	DB 4F
	DB 52
	DB 4C
	DB 44
	DB 21
	DB 00

END 
