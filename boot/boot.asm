[BITS 16]
[ORG 0x0]

jmp short start 
nop

; BIOS Parameter Block
bpbOEM: 				db "NOSBOOT "
bpbBytesPerSector: 		dw 512
bpbSectorsPerCluster: 	db 1
bpbReservedSectors: 	dw 1
bpbNumberOfFATs: 		db 2
bpbRootEntries: 		dw 224
bpbTotalSectors: 		dw 2880
bpbMedia: 				db 0xF8 
bpbSectorsPerFAT: 		dw 9
bpbSectorsPerTrack: 	dw 18
bpbHeadsPerCylinder: 	dw 2
bpbHiddenSectors: 		dd 0
bpbTotalSectorsBig: 	dd 0
bsDriveNumber: 			db 0
bsUnused: 				db 0
bsExtBootSignature: 	db 0x29 ; MSDOS 4.0 BPB
bsSerialNumber: 		dd 0x0C2D011B
bsVolumeLabel: 			db "NOS DISK   "
bsFileSystem: 			db "FAT12   "

start:
	; Initialize data segment
	cli
	mov ax, 0x07C0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; Initialize stack
	mov ax, 0x8000
	mov ss, ax
	mov sp, 0xF000
	sti

	mov si, MSG_REAL_MODE
	call print_string

	jmp load_root_directory

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

read_sectors:
.start:
	mov di, 0x5		; Number of retries
.main:
	push ax 
	push bx 
	push cx 
	call lba_to_chs 
	mov ah, 0x02	; BIOS read sector
	mov al, 0x01	; Read 1 sector
	mov ch, byte [absoluteTrack]
	mov cl, byte [absoluteSector]
	mov dh, byte [absoluteHead]
	mov dl, byte [bsDriveNumber]
	int 0x13
	jnc .success

	; Reset disk if error
	xor ax, ax
	int 0x13
	dec di 
	pop cx
	pop bx 
	pop ax 
	jnz .main
	int 0x18
.success:
	mov si, MSG_PROGRESS
	call print_string

	pop cx
	pop bx 
	pop ax 
	add bx, word [bpbBytesPerSector]
	inc ax 
	loop .start
	ret

lba_to_chs:
	xor dx, dx
	div word [bpbSectorsPerTrack] 	
	inc dl
	mov byte [absoluteSector], dl

	xor dx, dx
	div word [bpbHeadsPerCylinder]
	mov byte [absoluteHead], dl
	mov byte [absoluteTrack], al

	ret

chs_to_lba:
	sub ax, 0x2
	xor cx, cx
	mov cl, byte [bpbSectorsPerCluster]
	mul cx
	add ax, word [dataarea]
	ret

load_root_directory:
	; Get the size (number of sectors) of root directory and store it in cx
	xor cx, cx
	xor dx, dx
	mov ax, 0x20 							; 32 bytes per root entry
	mul word [bpbRootEntries] 				; AX = 32 * RootEntries
	div word [bpbBytesPerSector] 			; AX = AX / BytesPerSector
	xchg ax, cx

	; Get start of root directory
	; -----------------------------------------------------------------------------
	; Boot sector | Reserved sectors | FAT 1 | FAT 2 | Root directory | Data area
	; -----------------------------------------------------------------------------

	mov al, byte [bpbNumberOfFATs] 		; Number of FATs to skip
	mul word [bpbSectorsPerFAT] 		; AL = AL * SectorsPerFAT
	add ax, word [bpbReservedSectors] 	; AX = AX + ReservedSectors
	mov word [dataarea], ax
	add word [dataarea], cx

	; Load root directory at 7C00:0200 (i.e 0x7E00 sector after boot sector)
	mov bx, 0x200 
	call read_sectors

	; Browse root directory for stage 2
	mov cx, word [bpbRootEntries] 	; Number of entries to check
	mov di, 0x200 				; Address of root directory
.next_entry:
	push cx
	mov cx, 11 					; Number of characters of files in FAT12/16 FS
	mov si, LOADER_FILENAME
	push di
	rep cmpsb					; Compare current filename with loader filename

	pop di
	je load_fat 				; If found, load FAT

	pop cx
	add di, 0x20 				; Next entry
	loop .next_entry			; Decrement cx 

	jmp failure

load_fat:
	mov si, MSG_CRLF
	call print_string

	mov dx, word [di + 0x1A]		; Starting address of file entry + 26 bytes -> First cluster address
	mov word [cluster], dx

	; Number of sectors used by the FATs and store it in cx
	xor ax, ax 
	mov al, byte [bpbNumberOfFATs]
	mul word [bpbSectorsPerFAT]
	mov cx, ax

	; Load the FAT table at 0x7E00
	mov ax, word [bpbReservedSectors]
	mov bx, 0x0200
	call read_sectors

	; Read image file at 0050:0000
	mov si, MSG_CRLF
	call print_string
	mov ax, 0x50
	mov es, ax 
	mov bx , 0x0
	push bx 

load_image:
	mov ax, word [cluster]
	pop bx 
	call chs_to_lba

	xor cx, cx
	mov cl, byte [bpbSectorsPerCluster] ; sectors to read
	call read_sectors
	push bx 

	; Next cluster address
	mov ax, word [cluster]
	mov cx, ax 
	mov dx, ax 
	shr dx, 0x1 			; Divide by 2
	add cx, dx
	mov bx, 0x200 		; FAT table starts at 0x7E00
	add bx, cx 			; Index in FAT 
	mov dx, word [bx]	; Read 2 bytes
	test ax, 0x0001
	jnz .odd 
.even:
	and dx, 0x0FFF		; Take low 12 bits
	jmp .done
.odd:
	shr dx, 0x4		; Take high 12 bits
.done:
	mov word [cluster], dx
	cmp dx, 0xFF0		; End of file
	jb load_image

	; Jump to 0050:0000
	jmp dword 0x0050:0x0000

failure:
	mov si, MSG_FAILURE 
	call print_string
	
	mov ah, 0x0 
	int 0x16
	int 0x19	; Warm reboot

absoluteSector 	db 0x00
absoluteHead   	db 0x00
absoluteTrack  	db 0x00

dataarea 		dw 0x0000
cluster  		dw 0x0000

MSG_REAL_MODE: 	db "Started in 16-bit Real Mode", 13, 10, 0
MSG_PROGRESS: 	db ".", 0
MSG_CRLF: 		db 13, 10, 0
MSG_FAILURE: 	db "ERROR! Press any key to reboot", 13, 10, 0
LOADER_FILENAME: db "KRNLLDR SYS"

times 510 - ($ - $$) db 144 ; Fill rest of sector with NOP
dw 0xAA55 ; BIOS boot sector signature
