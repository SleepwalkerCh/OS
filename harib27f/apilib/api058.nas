[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api058.nas"]

		GLOBAL	_api_product1

[SECTION .text]

_api_product1:		; void api_product1()
		MOV		EDX,58
		INT		0x40
		RET