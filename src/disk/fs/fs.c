// fs.c
#include "fs.h"
#include "../../vga.h"
#include "../../stdlib/stdio.h"

void fs_init(uint8_t* initrd) {
    FAT32_BPB* bpb = (FAT32_BPB*) initrd;

    // Check signature
    if (bpb->bytesPerSector != 512) {
        printf("Unexpected sector size: %u\n", bpb->bytesPerSector);
        return;
    }

    puts("OEM: "); puts(bpb->OEMName); putc('\n');

    puts("Bytes/Sector: "); print_hex16(bpb->bytesPerSector); putc('\n');
    puts("Sectors/Cluster: "); print_hex8(bpb->sectorsPerCluster); putc('\n');
    puts("Reserved Sectors: "); print_hex16(bpb->reservedSectorCount); putc('\n');
    puts("Num FATs: "); print_hex8(bpb->numFATs); putc('\n');
    puts("Total Sectors: "); print_hex32(bpb->totalSectors32); putc('\n');
    puts("Sectors per FAT: "); print_hex32(bpb->FATSize32); putc('\n');
    puts("Root Cluster: "); print_hex32(bpb->rootCluster); putc('\n');
}
