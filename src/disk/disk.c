#include "disk.h"
#include "../util.h"  // Your inb/outb/inw/outw functions

// Primary ATA I/O ports
#define ATA_PRIMARY_DATA     0x1F0
#define ATA_PRIMARY_ERROR    0x1F1
#define ATA_PRIMARY_SECCOUNT 0x1F2
#define ATA_PRIMARY_LBA_LOW  0x1F3
#define ATA_PRIMARY_LBA_MID  0x1F4
#define ATA_PRIMARY_LBA_HIGH 0x1F5
#define ATA_PRIMARY_DRIVE    0x1F6
#define ATA_PRIMARY_STATUS   0x1F7
#define ATA_PRIMARY_CMD      0x1F7

#define ATA_CMD_READ_SECTORS 0x20

static void ata_wait() {
    // Wait until BSY=0 and DRQ=1
    // while ((inb(ATA_PRIMARY_STATUS) & 0x88) != 0x08);
}

void disk_init(void) {
    // For legacy PIO, no real init needed
    // You could detect disks here by reading IDENTIFY, optional
}

void disk_read_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait();

    outb(ATA_PRIMARY_SECCOUNT, 1); // read 1 sector
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F)); // master
    outb(ATA_PRIMARY_CMD, ATA_CMD_READ_SECTORS);

    ata_wait();

    uint16_t* buf16 = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) { // 256 words = 512 bytes
        buf16[i] = inw(ATA_PRIMARY_DATA);
    }
}

void disk_read_sectors(uint32_t lba, uint8_t* buffer, uint8_t count) {
    ata_wait();

    outb(ATA_PRIMARY_SECCOUNT, count);
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F)); // master
    outb(ATA_PRIMARY_CMD, ATA_CMD_READ_SECTORS);

    for (uint8_t s = 0; s < count; s++) {
        ata_wait();
        uint16_t* buf16 = (uint16_t*)(buffer + s * SECTOR_SIZE);
        for (int i = 0; i < 256; i++) {
            buf16[i] = inw(ATA_PRIMARY_DATA);
        }
    }
}
