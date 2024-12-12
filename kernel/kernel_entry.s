[bits 32]
[extern kernel_start]
GLOBAL _start

_start:
	call kernel_start
	jmp $
