AS = nasm
CC = i386-elf-gcc
LD = i386-elf-ld

CFLAGS = -ffreestanding -m32 -g -c
LDFLAGS = -Ttext 0x1000 --oformat binary

BUILD_DIR = build
BIN_DIR   = $(BUILD_DIR)/bin

BOOT_SRC    = src/bootloader/boot.asm
ZEROES_SRC  = src/bootloader/zeroes.asm
ENTRY_SRC   = src/kernel_entry.asm
KERNEL_SRC  = src/kernel.c
VGA_SRC     = src/vga.c

BOOT_BIN    = $(BIN_DIR)/boot.bin
ZEROES_BIN  = $(BIN_DIR)/zeroes.bin
ENTRY_OBJ   = $(BIN_DIR)/kernel_entry.o
KERNEL_OBJ  = $(BIN_DIR)/kernel.o
VGA_OBJ     = $(BIN_DIR)/vga.o
FULL_KERNEL = $(BIN_DIR)/full_kernel.bin
OS_BIN      = $(BUILD_DIR)/Doors-11-i386-elf.bin

all: $(OS_BIN)

$(BOOT_BIN): $(BOOT_SRC) | $(BIN_DIR)
	$(AS) $< -f bin -o $@

$(ZEROES_BIN): $(ZEROES_SRC) | $(BIN_DIR)
	$(AS) $< -f bin -o $@

$(ENTRY_OBJ): $(ENTRY_SRC) | $(BIN_DIR)
	$(AS) $< -f elf -o $@

$(KERNEL_OBJ): $(KERNEL_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(VGA_OBJ): $(VGA_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(FULL_KERNEL): $(ENTRY_OBJ) $(KERNEL_OBJ) $(VGA_OBJ)
	$(LD) -o $@ $(LDFLAGS) $(ENTRY_OBJ) $(KERNEL_OBJ) $(VGA_OBJ)

$(OS_BIN): $(BOOT_BIN) $(FULL_KERNEL) $(ZEROES_BIN)
	cat $(BOOT_BIN) $(FULL_KERNEL) $(ZEROES_BIN) > $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(OS_BIN)
	qemu-system-x86_64 -drive format=raw,file=$(OS_BIN),index=0,if=floppy -m 128M

clean:
	rm -rf $(BIN_DIR) $(OS_BIN)

.PHONY: all run clean
