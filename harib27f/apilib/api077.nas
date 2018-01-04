[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api077.nas"]

		GLOBAL	_api_readlist

[SECTION .text]

_api_readlist:		; int api_readlist(int i)
		PUSH		EBX
		MOV		EDX,77
		MOV		EBX,[ESP+8]
		INT		0x40
		POP		EBX
		RET