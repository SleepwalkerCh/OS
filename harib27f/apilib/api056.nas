[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api056.nas"]

		GLOBAL	_api_TASout

[SECTION .text]

_api_TASout:		; void api_TASout(int No)
		PUSH		EBX
		MOV		EDX,56
		MOV		EBX,[ESP+8]
		INT		0x40
		POP		EBX
		RET