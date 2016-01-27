all: bootload kernel loadFile

kernel.o: kernel.c
	bcc -ansi -c kernel.c -o kernel.o

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

kernel:kernel.o kernel_asm.o
	ld86 -o kernel -d kernel.o kernel_asm.o

loadFile: loadFile.c
	gcc -o loadFile loadFile.c

bootload: bootload.asm
	nasm bootload.asm

clean:
	rm -rf *.o kernel bootload loadFile

run:
	dd if=/dev/zero of=floppya.img bs=512 count=2880
	dd if=bootload of=floppya.img bs=512 conv=notrunc count=1
	dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
	dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
	dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
	./loadFile message.txt
	bochs -f opsys.bxrc
