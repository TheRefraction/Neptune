extern isr_default, isr_clock, isr_keyboard
global _asm_default_irq, _asm_irq_0, _asm_irq_1

_asm_default_irq:
    call isr_default
    mov al, 0x20
    out 0x20, al
    iret

_asm_irq_0:
    call isr_clock
    mov al, 0x20
    out 0x20, al
    iret

_asm_irq_1:
    call isr_keyboard
    mov al, 0x20
    out 0x20, al
    iret
