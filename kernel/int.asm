extern isr_default, isr_DE_ex, isr_DB_ex, isr_NMI_ex, isr_BP_ex, isr_OF_ex, isr_BR_ex, isr_UD_ex, isr_NM_ex, isr_DF_ex, isr_TS_ex, isr_NP_ex, isr_SS_ex, isr_GP_ex, isr_PF_ex, isr_clock, isr_keyboard, do_syscalls
global _asm_default_irq, _asm_ex_DE, _asm_ex_DB, _asm_ex_NMI, _asm_ex_BP, _asm_ex_OF, _asm_ex_BR, _asm_ex_UD, _asm_ex_NM, _asm_ex_DF, _asm_ex_TS, _asm_ex_NP, _asm_ex_SS, _asm_ex_GP, _asm_ex_PF, _asm_irq_0, _asm_irq_1, _asm_syscalls

%macro SAVE_REGS 0
	pushad
	push ds
	push es
	push fs
	push gs
	push ebx
	mov bx, 0x10
	mov ds, bx
	pop ebx
%endmacro

%macro RESTORE_REGS 0
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

_asm_ex_DE:
  SAVE_REGS
  call isr_DE_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_DB:
  SAVE_REGS
  call isr_DB_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_NMI:
  SAVE_REGS
  call isr_NMI_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_BP:
  SAVE_REGS
  call isr_BP_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_OF:
  SAVE_REGS
  call isr_OF_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_BR:
  SAVE_REGS
  call isr_BR_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_UD:
  SAVE_REGS
  call isr_UD_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_NM:
  SAVE_REGS
  call isr_NM_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_DF:
  SAVE_REGS
  call isr_DF_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_TS:
  SAVE_REGS
  call isr_TS_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_NP:
  SAVE_REGS
  call isr_NP_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_SS:
  SAVE_REGS
  call isr_SS_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_GP:
  SAVE_REGS
  call isr_GP_ex
  RESTORE_REGS
  add esp, 4
  iret

_asm_ex_PF:
  SAVE_REGS
  call isr_PF_ex
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

_asm_syscalls:
  SAVE_REGS
  push eax
  call do_syscalls
  pop eax
  RESTORE_REGS
  iret

