#include <stdio.h>
#include "../include/stdbool.h"
#include "core32.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filesystem.img>\n", argv[0]);
        return -1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    superblock super;

    // Read superblock
    if (fread(&super, sizeof(superblock), 1, fp) != 1) {
        fprintf(stderr, "Failed to read superblock\n");
        fclose(fp);
        return -1;
    }

    // Verify superblock
    if (super.magic != FS_MAGIC) {
        fprintf(stderr, "Invalid filesystem (bad magic: 0x%X)\n", super.magic);
        fclose(fp);
        return -1;
    }
    if (super.block_size != FS_BLOCK_SIZE) {
        fprintf(stderr, "Unexpected block size!\n");
        fclose(fp);
        return -1;
    }
    if (super.max_blocks == 0 || super.max_blocks > FS_MAX_BLOCKS) {
        fprintf(stderr, "Invalid max blocks %u\n", super.max_blocks);
        fclose(fp);
        return -1;
    }
    if (super.max_inodes == 0 || super.max_inodes > FS_MAX_INODES) {
        fprintf(stderr, "Invalid max inodes %u\n", super.max_inodes);
        fclose(fp);
        return -1;
    }

    printf("Filesystem verified!\n");
    printf("Blocks: %u, Inodes: %u\n", super.max_blocks, super.max_inodes);

    fclose(fp);
    return 0;
}
