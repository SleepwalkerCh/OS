[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api078.nas"]

		GLOBAL	_api_writelist

[SECTION .text]

_api_writelist:		; void api_writelist(int x, int y)
		PUSH		EBX
		PUSH		ESI
		MOV		EDX,78
		MOV		EBX,[ESP+12]
		MOV		ESI,[ESP+16]
		INT		0x40
		POP		EBX
		POP		ESI
		RET