[bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

; prints a null terminated string pointed to by EBX
print_string_pm:
	pusha
	mov edx, VIDEO_MEMORY

print_string_pm_main:
	mov al, [ebx]
	mov ah, WHITE_ON_BLACK

	cmp al, 0
	je print_string_pm_end

	mov [edx], ax
	inc ebx ; Get next char in strig
	add edx, 2 ; Move to next char cel in video memory
	jmp print_string_pm_main

print_string_pm_end:
	popa
	ret
