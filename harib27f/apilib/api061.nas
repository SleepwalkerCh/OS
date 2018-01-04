[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api061.nas"]

		GLOBAL	_api_product3

[SECTION .text]

_api_product3:		; void api_product3()
		MOV		EDX,61
		INT		0x40
		RET