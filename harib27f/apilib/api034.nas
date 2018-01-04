[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api034.nas"]

		GLOBAL	_api_consumer

[SECTION .text]

_api_consumer:		; void api_consumer()
		MOV		EDX,34
		INT		0x40
		RET