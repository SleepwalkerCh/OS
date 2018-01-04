[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api033.nas"]

		GLOBAL	_api_producer

[SECTION .text]

_api_producer:		; void api_producer()
		MOV		EDX,33
		INT		0x40
		RET
