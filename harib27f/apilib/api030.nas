[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api030.nas"]

		GLOBAL	_api_num1

[SECTION .text]

_api_num1:		; void api_num1()
		MOV		EDX,30
		INT		0x40
		RET
