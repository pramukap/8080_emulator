LETTER 	EQU 42
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

;BITS	4	3	2	1	0
;FLAGS	DONE	WRITE	READ	RDY	INTE

;Store data to nv data register
MVI A, LETTER
STA DATA

;Store address to address register
MVI A, ADRL
STA M_ADR_L

MVI A, ADRH
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

FIN:	HLT

END 
