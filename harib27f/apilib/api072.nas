[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api072.nas"]

		GLOBAL	_api_getfull

[SECTION .text]

_api_getfull:		; int* api_getfull()
		PUSH		EBX
		MOV		EDX,72
		INT		0x40
		POP		EBX
		RET