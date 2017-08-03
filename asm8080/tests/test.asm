HELLO_WORLD:	mvi h, 12; 0,1
		adi 0 ;2,3
		jmp LABEL ; 4,5,6
L1:		adi 1 ; 7
		nop ; 8
LABEL:		nop ; 9
		jz L1 ; a,b,c
		hlt ;d	
