[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api051.nas"]

		GLOBAL	_api_createrownum

[SECTION .text]

_api_createrownum:		; int api_createrownum()
		PUSH		EBX
		MOV		EDX,51
		INT		0x40
		POP		EBX
		RET