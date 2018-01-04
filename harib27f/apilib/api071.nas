[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api071.nas"]

		GLOBAL	_api_getmutex

[SECTION .text]

_api_getmutex:		; int* api_getmutex()
		PUSH		EBX
		MOV		EDX,71
		INT		0x40
		POP		EBX
		RET