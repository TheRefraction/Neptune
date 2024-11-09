[bits 32]
[extern kernel_main]
GLOBAL _start

_start:
	call kernel_main
	jmp $
