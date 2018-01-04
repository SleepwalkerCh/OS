[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api053.nas"]

		GLOBAL	_api_TASin

[SECTION .text]

_api_TASin:		; void api_TASin(int No)
		PUSH		EBX
		MOV		EDX,53
		MOV		EBX,[ESP+8]
		INT		0x40
		POP		EBX
		RET