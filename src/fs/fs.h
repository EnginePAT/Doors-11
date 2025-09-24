// fs.h
#ifndef FS_H
#define FS_H

#include "../stdint.h"

typedef struct {
    uint8_t  jmpBoot[3];        // Jump instruction (not important)
    char     OEMName[8];        // OEM string
    uint16_t bytesPerSector;    // Typically 512
    uint8_t  sectorsPerCluster; // Cluster size (1,2,4... sectors)
    uint16_t reservedSectorCount;
    uint8_t  numFATs;           // Usually 2
    uint16_t rootEntryCount;    // 0 for FAT32
    uint16_t totalSectors16;
    uint8_t  media;
    uint16_t FATSize16;         // 0 for FAT32
    uint16_t sectorsPerTrack;
    uint16_t numHeads;
    uint32_t hiddenSectors;
    uint32_t totalSectors32;

    // FAT32 extended part
    uint32_t FATSize32;         // Sectors per FAT
    uint16_t extFlags;
    uint16_t FSVersion;
    uint32_t rootCluster;       // Usually 2
    uint16_t FSInfo;
    uint16_t backupBootSector;
    uint8_t  reserved[12];
    uint8_t  driveNumber;
    uint8_t  reserved1;
    uint8_t  bootSignature;     // 0x29 means serial/label available
    uint32_t volumeID;
    char     volumeLabel[11];
    char     fsType[8];         // "FAT32   "
} __attribute__((packed)) FAT32_BPB;

void fs_init(uint8_t* initrd);

#endif
