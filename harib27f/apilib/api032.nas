[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api032.nas"]

		GLOBAL	_api_num3

[SECTION .text]

_api_num3:		; void api_num3()
		MOV		EDX,32
		INT		0x40
		RET
