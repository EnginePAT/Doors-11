#include "disk.h"
#include "../vga.h"

void disk_init(multiboot_module_t* module) {
    if (!module) return;

    uintptr_t start = (uintptr_t)module->mod_start;
    uintptr_t end   = (uintptr_t)module->mod_end;

    if (end <= start) {
        print("Invalid module addresses!\n");
        disk.data = 0;
        disk.size = 0;
        return;
    }

    disk.data = (uint8_t*)start;
    disk.size = (uint32_t)(end - start);
}

uint8_t disk_read_byte(uint32_t offset) {
    if (offset >= disk.size) return 0;
    return disk.data[offset];
}

void disk_read_bytes(uint32_t offset, uint8_t* buffer, uint32_t length) {
    if (offset + length > disk.size) length = disk.size - offset;
    for (uint32_t i = 0; i < length; i++) {
        buffer[i] = disk.data[offset + i];
    }
}
