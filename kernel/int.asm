extern isr_default, isr_GP_ex, isr_clock, isr_keyboard
global _asm_default_irq, _asm_ex_GP, _asm_irq_0, _asm_irq_1

%macro	SAVE_REGS 0
	pushad 
	push ds
	push es
	push fs
	push gs 
	push ebx
	mov bx,0x10
	mov ds,bx
	pop ebx
%endmacro

%macro	RESTORE_REGS 0
	pop gs
	pop fs
	pop es
	pop ds
	popad
%endmacro

_asm_default_irq:
    SAVE_REGS
    call isr_default
    mov al, 0x20
    out 0x20, al
    RESTORE_REGS
    iret

_asm_ex_GP:
    SAVE_REGS
    call isr_GP_ex
    RESTORE_REGS
    add esp, 4
    iret

_asm_irq_0:
    SAVE_REGS
    call isr_clock
    mov al, 0x20
    out 0x20, al
    RESTORE_REGS
    iret

_asm_irq_1:
    SAVE_REGS
    call isr_keyboard
    mov al, 0x20
    out 0x20, al
    RESTORE_REGS
    iret
