#ifndef DISK_H
#define DISK_H

#include "stdint.h"

// Maximum sector size
#define SECTOR_SIZE 512

// Initialize the primary ATA channel
void disk_init(void);

// Read a single 512-byte sector from primary master
// lba: logical block address
// buffer: pointer to memory (at least 512 bytes)
void disk_read_sector(uint32_t lba, uint8_t* buffer);

// Optional: read multiple sectors (count <= 255)
void disk_read_sectors(uint32_t lba, uint8_t* buffer, uint8_t count);

#endif // DISK_H
