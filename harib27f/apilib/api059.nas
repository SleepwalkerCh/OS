[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api059.nas"]

		GLOBAL	_api_product2

[SECTION .text]

_api_product2:		; void api_product2()
		MOV		EDX,59
		INT		0x40
		RET