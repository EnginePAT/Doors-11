#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <multiboot.h>

typedef struct {
    uint8_t* data;
    uint32_t size;
} Disk;

static Disk disk;

// Initialize the disk with a GRUB module
void disk_init(multiboot_module_t* module);

// Read a single byte
uint8_t disk_read_byte(uint32_t offset);

// Read multiple bytes into a buffer
void disk_read_bytes(uint32_t offset, uint8_t* buffer, uint32_t length);

#endif
