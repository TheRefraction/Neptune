%define KERNEL_OFFSET 0x2000

[BITS 16]
[ORG 0x0]

jmp start

%include "util.inc"

start:
	mov ax, 0x100
	mov ds, ax
	mov es, ax

	mov ax, 0x8000
	mov ss, ax
	mov sp, 0xf000

	mov [BOOT_DRIVE], dl

	mov si, MSG_LOADER
	call print_string

	call enable_a20
	jc a20_failure

	call load_kernel
	call load_gdt
	call switch_to_pm

	jmp $ ; Halt forever

load_kernel:
	mov si, MSG_LOAD_KERNEL
	call print_string

	xor ax, ax
	int 0x13

	push es
	mov ax, 0
	mov es, ax
	mov bx, KERNEL_OFFSET

	mov ah, 0x02
	mov al, 15
	mov ch, 0
	mov cl, 4	
	mov dh, 0
	mov dl, [BOOT_DRIVE]
	int 0x13	
	jc .load_error

	cmp al, 15 ; Did we read enough sectors?
	jne .load_error

	pop es

	ret
.load_error:
	mov si, MSG_LOAD_ERROR
	call print_string
	jmp $

load_gdt:
	mov si, MSG_LOAD_GDT
	call print_string

	; Compute the limit
	mov ax, gdt_end
	mov bx, gdt
	sub ax, bx
	mov word [gdt_descriptor], ax

	; Compute the linear address
	xor eax, eax
	xor ebx, ebx
	mov ax, ds
	mov ecx, eax
	shl ecx, 4
	mov bx, gdt
	add ecx, ebx
	mov dword [gdt_descriptor+2], ecx

	ret

switch_to_pm:
	cli ; Disable BIOS Interrupts
	lgdt [gdt_descriptor] ; Load new GDT to the CPU

	; Update first bit of CR0 Register
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp protected_mode

a20_failure:
	mov si, MSG_A20_ERROR
	call print_string
	jmp $

protected_mode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Initialize new stack
	mov ebp, 0x90000
	mov esp, ebp

	mov ebx, MSG_PROT_MODE
	call print_string_pm

	jmp dword 0x8:KERNEL_OFFSET ; Far jump to Kernel entry point

BOOT_DRIVE: db 0
MSG_PROT_MODE: db "Entered 32-bit Protected Mode!", 0
MSG_A20_ERROR: db "A20 error.", 13, 10, 0
MSG_LOADER: db "Entered Stage 2.", 13, 10, 0
MSG_LOAD_KERNEL: db "Loading Kernel into memory.", 13, 10, 0
MSG_LOAD_GDT: db "Loading GDT", 13, 10, 0
MSG_LOAD_ERROR: db "Error! Kernel could not be loaded!", 13, 10, 0

gdt: db 0, 0, 0, 0, 0, 0, 0, 0
gdt_code: db 0xff, 0xff, 0x0, 0x0, 0x0, 10011011b, 11011111b, 0x0
gdt_data: db 0xff, 0xff, 0x0, 0x0, 0x0, 10010011b, 11011111b, 0x0
gdt_end:

gdt_descriptor: dw 0
		dd 0

times 1024 - ($-$$) db 144
