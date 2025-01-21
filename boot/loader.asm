[BITS 16]
[ORG 0x0]

jmp start

%include "util.inc"
%include "cons.inc"

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

  call detect_upper_memory
  mov ax, 0x100 
  mov ds, ax 
  mov es, ax

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
	mov bx, 0x2000

	mov ah, 0x02
	mov al, KERNEL_SIZE
	mov ch, 0
	mov cl, 2 + LOADER_SIZE
	mov dh, 0
	mov dl, [BOOT_DRIVE]
	int 0x13
	jc .load_error

	cmp al, KERNEL_SIZE ; Did we read enough sectors?
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

detect_upper_memory:
  mov ax, 0x0 
  mov es, ax 
  mov di, 0x7000 

  mov ebx, 0x0 

.next_entry:
  mov eax, 0xE820 
  mov edx, 0x534D4150
  mov ecx, 24

  int 0x15
  jc .failed

  cmp eax, 0x534D4150 
  jne .failed 

  test ebx, ebx 
  jz .end

  add di, 24
  jmp .next_entry 

.end:
  mov ax, 0x0 
  mov ds, ax 
  mov si, 0x7000 

  mov eax, dword [es:di]
  mov dword [ds:si], eax

  add di, 8 
  add si, 4 

  mov eax, dword [es:di]
  mov dword [ds:si], eax

  ret

.failed:
	mov si, MSG_MEM_ERROR 
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

  mov esi, 0x2000
  mov edi, KERNEL_OFFSET
  mov cx, 256 * KERNEL_SIZE ; Number of words to copy (2 bytes)

copy_kernel:
  mov eax, [ds:esi]
  mov [es:edi], ax

  add esi, 2
  add edi, 2

  loop copy_kernel

	jmp dword 0x8:KERNEL_OFFSET ; Far jump to Kernel entry point

BOOT_DRIVE: db 0
MSG_PROT_MODE: db "Entered 32-bit Protected Mode!", 0
MSG_A20_ERROR: db "Error! A20 Line could not be enabled", 13, 10, 0
MSG_LOADER: db "Entered Stage 2.", 13, 10, 0
MSG_LOAD_KERNEL: db "Loading Kernel into memory.", 13, 10, 0
MSG_LOAD_GDT: db "Loading GDT", 13, 10, 0
MSG_LOAD_ERROR: db "Error! Kernel could not be loaded!", 13, 10, 0
MSG_MEM_ERROR: db "Error! Could not get upper memory limit!", 13, 10, 0

gdt: db 0, 0, 0, 0, 0, 0, 0, 0
gdt_code: db 0xff, 0xff, 0x0, 0x0, 0x0, 10011011b, 11011111b, 0x0
gdt_data: db 0xff, 0xff, 0x0, 0x0, 0x0, 10010011b, 11011111b, 0x0
gdt_end:

gdt_descriptor: dw 0
                dd 0

times LOADER_SIZE * 512 - ($-$$) db 144
