[BITS 16]
[ORG 0x0]

jmp start

%include "cons.inc"

start:
	; Initialize segments
	mov ax, 0x07c0
	mov ds, ax
	mov es, ax

	; Initialize stack
	mov ax, 0x8000
	mov ss, ax
	mov sp, 0xf000

	mov [BOOT_DRIVE], dl

	mov si, MSG_REAL_MODE
	call print_string

	; Load the larger part of the bootloader (stage 2)
	xor ax, ax
	int 0x13

	push es
	mov ax, LOADER_BASE
	mov es, ax
	mov bx, 0

	mov ah, 2
	mov al, LOADER_SIZE ; Stage 2 is 2 sectors (1024 bytes)
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, [BOOT_DRIVE]
	int 0x13

	pop es

	; Jump to stage 2
	jmp dword LOADER_BASE:0

print_string:
	push ax
	push bx
.main:
	lodsb
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
MSG_REAL_MODE: db "Started in 16-bit Real Mode", 13, 10, 0

times 510 - ($ - $$) db 144 ; Fill rest of sector with NOP
dw 0xAA55 ; BIOS boot sector signature
