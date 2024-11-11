%define BASE 0x100

[BITS 16]
[ORG 0x0]

jmp start

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

	;mov si, MSG_REAL_MODE
	;call print_string

	; Load the larger part of the bootloader (stage 2)
	xor ax, ax
	int 0x13

	push es
	mov ax, BASE
	mov es, ax
	mov bx, 0

	mov ah, 2
	mov al, 2 ; Stage 2 is 2 sectors (1024 bytes)
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, [BOOT_DRIVE]
	int 0x13

	pop es

	; Jump to stage 2
	jmp dword BASE:0


BOOT_DRIVE: db 0
MSG_REAL_MODE: db "Started in 16-bit Real Mode", 13, 10, 0

times 510 - ($ - $$) db 144 ; Fill rest of sector with NOP
dw 0xAA55 ; BIOS boot sector signature
