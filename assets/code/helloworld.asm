; "helloworld.asm"
; This is an example Hello World assembly file
	org 1000h

	; signature, byte count, checksum
	DB 5Eh,00h,00h,00h,00h

	; name of the cartridge (11 bytes)
	DB 00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h

	; display Hello World
	jp printmsg

message:
	DB "Hello World!",255

printmsg:
	ld hl,message
	ld bc,5000h
print:
	ld a,(hl)
	cp 255
	jr Z,loop
	ld (bc),a
	inc hl
	inc bc
	jp print

	; set infinite loop
loop:
	jp loop