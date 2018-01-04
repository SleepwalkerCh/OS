[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api054.nas"]

		GLOBAL	_api_createcompete1

[SECTION .text]

_api_createcompete1:		; int api_createcompete1()
		PUSH		EBX
		MOV		EDX,54
		INT		0x40
		POP		EBX
		RET