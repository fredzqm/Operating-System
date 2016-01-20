# if [ ! -f bootload ]; then
# 	make bootload
# fi
# if [ ! -f kernel ]; then
# 	make kernel
# fi

# dd if=/dev/zero of=floppya.img bs=512 count=2880
# dd if=bootload of=floppya.img bs=512 conv=notrunc count=1 
# dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
# bochs -f opsys.bxrc

# bcc -ansi -c -o kernel.o kernel.c
# as86 kernel.asm -o kernel_asm.o
# ld86 -o kernel -d kernel.o kernel_asm.o
# dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3