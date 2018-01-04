[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api076.nas"]

		GLOBAL	_api_getcount

[SECTION .text]

_api_getcount:		; int api_getcount()
		PUSH		EBX
		MOV		EDX,76
		INT		0x40
		POP		EBX
		RET