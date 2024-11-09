; Initializes the GDT descriptor
gdt_start:

gdt_null: db 0, 0, 0, 0, 0, 0, 0, 0
gdt_code: db 0xff, 0xff, 0x0, 0x0, 0x0, 10011011b, 11011111b, 0x0
gdt_data: db 0xff, 0xff, 0x0, 0x0, 0x0, 10010011b, 11011111b, 0x0

gdt_end:

gdt_descriptor: dw gdt_end - gdt_start - 1
				dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
