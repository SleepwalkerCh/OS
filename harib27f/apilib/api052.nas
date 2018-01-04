[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api052.nas"]

		GLOBAL	_api_createmaxrow

[SECTION .text]

_api_createmaxrow:		; int api_createmaxrow()
		PUSH		EBX
		MOV		EDX,52
		INT		0x40
		POP		EBX
		RET