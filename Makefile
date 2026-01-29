# Makefile for ParadoxOS

CC=gcc
AS=nasm
LD=ld

# Adjust flags if cross-compiling, e.g. i686-elf-gcc
CFLAGS=-m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS=-felf32
LDFLAGS=-melf_i386 -T linker.ld

KERNEL_OBJ=src/kernel/kernel.o
BOOT_OBJ=src/boot/boot.o
TARGET=paradox.bin
ISO=paradox.iso

all: $(TARGET)

$(TARGET): $(BOOT_OBJ) $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

src/boot/boot.o: src/boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

src/kernel/kernel.o: src/kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/kernel/*.o src/boot/*.o $(TARGET) $(ISO)

iso: $(TARGET)
	mkdir -p iso/boot/grub
	cp $(TARGET) iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "ParadoxOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/paradox.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso
	rm -rf iso
