[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api057.nas"]

		GLOBAL	_api_createcompete2

[SECTION .text]

_api_createcompete2:		; int api_createcompete2()
		PUSH		EBX
		MOV		EDX,57
		INT		0x40
		POP		EBX
		RET