[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api031.nas"]

		GLOBAL	_api_num2

[SECTION .text]

_api_num2:		; void api_num2()
		MOV		EDX,31
		INT		0x40
		RET
