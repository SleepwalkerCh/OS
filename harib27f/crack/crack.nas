[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "hello5.nas"]

		GLOBAL	_HariMain

[SECTION .text]

_HariMain:
		MOV		EDX,123456789
		INT		0x40
		MOV		EDX,4
		INT		0x40


