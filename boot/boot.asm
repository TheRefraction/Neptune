[org 0x7c00]

KERNEL_OFFSET equ 0x1000

; Code starts here
mov [BOOT_DRIVE], dl

; Initialize the stack 
mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print_string

call load_kernel

call switch_to_pm

jmp $ ; Halt forever

switch_to_pm:
	cli ; Disable BIOS interrupts
	lgdt [gdt_descriptor] ; Load new GDT to the CPU

	; Update first bit of cr0 register ==> Informs the CPU we're switching to PM 
	mov eax, cr0
	or eax, 1
	mov cr0, eax 

	jmp CODE_SEG:init_protected_mode ; Far jump

%include "include/print_string.asm"
%include "include/print_string_pm.asm"
%include "include/gdt.asm"
%include "include/disk_read.asm"

[bits 16]

load_kernel:
	mov bx, MSG_LOAD_KERNEL
	call print_string

	mov bx, KERNEL_OFFSET
	mov dh, 15
	mov dl, [BOOT_DRIVE]
	call disk_load

	ret

[bits 32]

init_protected_mode:
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Initialize new stack
	mov ebp, 0x90000
	mov esp, ebp

start_protected_mode:
	mov ebx, MSG_PROT_MODE
	call print_string_pm

	call KERNEL_OFFSET ; Jump to Kernel entry point

	jmp $ ; Halt forever

BOOT_DRIVE db 0
MSG_REAL_MODE db "Started N/OS in 16-bit Real Mode.", 13, 10, 0
MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode!", 0
MSG_LOAD_KERNEL db "Loading N/OS Kernel into memory.", 13, 10, 0

times 510 - ($-$$) db 144 ; Fills first sector with NOP opcode until we have 512KB
dw 0xAA55 ; BIOS Sector boot magic word
