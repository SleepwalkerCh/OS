[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api050.nas"]

		GLOBAL	_api_createrowNo

[SECTION .text]

_api_createrowNo:		; int api_createrowNo()
		PUSH		EBX
		MOV		EDX,50
		INT		0x40
		POP		EBX
		RET