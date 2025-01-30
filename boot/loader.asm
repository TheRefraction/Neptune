[BITS 16]
[ORG 0x0]

jmp main

main:
    cli
    push cs 
    pop ds ; ds = cs

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

MSG_LOADER: db "Neptune Loader", 13, 10, 0