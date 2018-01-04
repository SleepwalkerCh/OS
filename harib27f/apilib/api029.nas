[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api029.nas"]

		GLOBAL	_api_min1

[SECTION .text]

_api_min1:		; void api_min1()
		MOV		EDX,29
		INT		0x40
		RET
