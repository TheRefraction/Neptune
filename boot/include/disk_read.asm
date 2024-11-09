; load DH sectors to ES:BX from drive DL
disk_load:
	push dx

	mov ah, 2
	mov al, dh

	mov ch, 0
	mov dh, 0
	mov cl, 2

	int 0x13
	jc disk_error

	pop dx
	cmp dh, al ; Did we read the right amount of sectors?
	jne disk_error

	ret

disk_error:
	mov bx, DISK_ERROR_MSG
	call print_string
	jmp $ ; Halt forever

DISK_ERROR_MSG db "Disk read error!", 0
