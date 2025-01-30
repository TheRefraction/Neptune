[BITS 16]
[ORG 0x0]

jmp main

main:
    cli
    push cs 
    pop ds ; ds = cs

	; Clear screen
	mov ah, 0x0
	int 0x10

    mov si, MSG_LOADER 
    call print_string

    jmp $

print_string:
	push ax
	push bx
.main:
	lodsb ; ds:si -> al
	cmp al, 0
	jz .end

	mov ah, 0x0e
	mov bx, 0x07
	int 0x10
	jmp .main

.end:
	pop bx
	pop ax
	ret

BOOT_DRIVE: db 0
MSG_LOADER: db "Neptune Loader", 13, 10, 0
MSG_PROT_MODE: db "Entered 32-bit Protected Mode!", 0
MSG_A20_ERROR: db "Error! A20 Line could not be enabled", 13, 10, 0
MSG_LOAD_KERNEL: db "Loading Kernel into memory.", 13, 10, 0
MSG_LOAD_GDT: db "Loading GDT", 13, 10, 0
MSG_LOAD_ERROR: db "Error! Kernel could not be loaded!", 13, 10, 0
MSG_MEM_ERROR: db "Error! Could not get upper memory limit!", 13, 10, 0

gdt: db 0, 0, 0, 0, 0, 0, 0, 0
gdt_code: db 0xFF, 0xFF, 0x0, 0x0, 0x0, 10011011b, 11011111b, 0x0
gdt_data: db 0xFF, 0xFF, 0x0, 0x0, 0x0, 10010011b, 11011111b, 0x0
gdt_end:

gdt_descriptor: dw 0
                dd 0