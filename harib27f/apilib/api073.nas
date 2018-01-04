[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api073.nas"]

		GLOBAL	_api_getlist

[SECTION .text]

_api_getlist:		; int* api_getlist()
		PUSH		EBX
		MOV		EDX,73
		INT		0x40
		POP		EBX
		RET