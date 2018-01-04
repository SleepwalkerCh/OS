[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api070.nas"]

		GLOBAL	_api_getempty

[SECTION .text]

_api_getempty:		; int* api_getempty()
		PUSH		EBX
		MOV		EDX,70
		INT		0x40
		POP		EBX
		RET