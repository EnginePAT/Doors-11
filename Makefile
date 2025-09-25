gcc=i386-elf-gcc
ld=i386-elf-ld
CFLAGS = -m32 -ffreestanding -g -nostdlib -I src/include

all: clean loc kernel boot image

clean:
	rm -rf *.o

loc:
	find src -name "*.c" -o -name "*.h" -o -name "*.s" | xargs wc -l

kernel:
	$(gcc) $(CFLAGS) -c src/kernel.c -o kernel.o
	$(gcc) $(CFLAGS) -c src/vga.c -o vga.o
	$(gcc) $(CFLAGS) -c src/memory/gdt/gdt.c -o gdt.o
	$(gcc) $(CFLAGS) -c src/util.c -o util.o
	$(gcc) $(CFLAGS) -c src/memory/interrupts/idt.c -o idt.o
	$(gcc) $(CFLAGS) -c src/timer.c -o timer.o
	$(gcc) $(CFLAGS) -c src/stdlib/stdio.c -o stdio.o
	$(gcc) $(CFLAGS) -c src/keyboard.c -o keyboard.o
	$(gcc) $(CFLAGS) -c src/memory/memory.c -o memory.o
	$(gcc) $(CFLAGS) -c src/memory/kmalloc.c -o kmalloc.o
	$(gcc) $(CFLAGS) -c src/disk/fs/fs.c -o fs.o
	$(gcc) $(CFLAGS) -c src/string.c -o string.o
	$(gcc) $(CFLAGS) -c src/disk/disk.c -o disk.o

boot:
	nasm -f elf32 src/boot.s -o boot.o
	nasm -f elf32 src/memory/gdt/gdt.s -o gdts.o
	nasm -f elf32 src/memory/interrupts/idt.s -o idts.o
image:
	$(ld) -T src/linker.ld -o kernel boot.o kernel.o vga.o gdt.o gdts.o util.o idt.o idts.o \
		timer.o stdio.o keyboard.o memory.o kmalloc.o fs.o string.o disk.o
	mv kernel Doors/boot/kernel
	grub-mkrescue -o doors-11-i386-elf.iso Doors/
	rm *.o

run:
	qemu-system-i386 -hda doors-11-i386-elf.iso
