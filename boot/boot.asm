[BITS 16]
[ORG 0x0]

mov ax, 0x07C0
mov ds, ax
mov es, ax
mov ax, 0x8000
mov ss, ax
mov sp, 0xf000

mov si, msgTest
call display

end:
	jmp end

msgTest db "Hello World !", 13, 10, 0

display:
	push ax
	push bx
.start:
	lodsb
	cmp al, 0
	jz .end
	mov ah, 0x0E
	mov bx, 0x07
	int 0x10
	jmp .start

.end:
	pop bx
	pop ax
	ret

times 510-($-$$) db 144
dw 0xAA55
