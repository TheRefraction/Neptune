; prints a null terminated string pointed to by BX
print_string:
	pusha
	mov ah, 0x0e ; Teletype mode

print_string_main:
	mov al, [bx]

	cmp al, 0 ; Are we at the end of the string?
	je print_string_end

	int 0x10 ; Display character loaded into al
	inc bx ; Moving forward in the string
	jmp print_string_main

print_string_end:
	popa
	ret
