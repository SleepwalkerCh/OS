[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api075.nas"]

		GLOBAL	_api_signal

[SECTION .text]

_api_signal:		; void api_signal(int *sem)
		PUSH		EBX
		MOV		EDX,75
		MOV		EBX,[ESP+8]
		INT		0x40
		POP		EBX
		RET