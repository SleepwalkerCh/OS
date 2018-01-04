[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api074.nas"]

		GLOBAL	_api_wait

[SECTION .text]

_api_wait:		; void api_wait(int *sem)
		PUSH		EBX
		MOV		EDX,74
		MOV		EBX,[ESP+8]
		INT		0x40
		POP		EBX
		RET