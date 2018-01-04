[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api060.nas"]

		GLOBAL	_api_createcompete3

[SECTION .text]

_api_createcompete3:		; int api_createcompete3()
		PUSH		EBX
		MOV		EDX,60
		INT		0x40
		POP		EBX
		RET