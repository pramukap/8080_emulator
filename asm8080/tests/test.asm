ORG 000a

HELLO_WORLD:	mvi h, 12; 0,1
		adi 0 ;2,3
		jmp LABEL ; 4,5,6
L1:		adi 1 ; 7,8
		nop ; 9
LABEL:		nop ; a
		jz L1 ; b,c,d
		hlt ;e	
