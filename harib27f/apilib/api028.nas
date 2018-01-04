[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api028.nas"]

		GLOBAL	_api_add1

[SECTION .text]

_api_add1:		; void api_add1()
		MOV		EDX,28
		INT		0x40
		RET
