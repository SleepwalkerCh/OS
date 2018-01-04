[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api055.nas"]

		GLOBAL	_api_cons_putstr0

[SECTION .text]

_api_cons_putstr0:		; void api_cons_putstr0()
		MOV		EDX,55
		INT		0x40
		RET