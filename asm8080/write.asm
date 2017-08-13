LETTER EQU 42

;Store data to nv data register
MVI A, LETTER
STA 3ffd

;Store address to address register
MVI A, 01
STA 3ffe

MVI A, 00
STA 3fff

;Modify control register to write to storage
LDA 3ffc
ORI 08
STA 3ffc

;Wait for storage to happen
WAIT: 	LDA 3ffc
	ANI 02
	JZ WAIT

HLT

END

HLT
 
