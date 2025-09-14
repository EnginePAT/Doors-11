#include "../include/stdint.h"

#define FS_BLOCK_SIZE       1024
#define FS_MAGIC            0xED6F9
#define FS_SUPERBLOCK_BLOCK 0
#define FS_INODE_TABLE_BLOCK 1
#define FS_MAX_FILENAME     255
#define FS_MAX_BLOCKS       1024
#define FS_MAX_INODES       128

// File types (stored in upper bits of MODE)
#define FS_TYPE_MASK        0xF0
#define FS_FILE_TYPE_FILE   0x10
#define FS_FILE_TYPE_DIR    0x20

// Permissions (stored in lower bits of MODE)
#define FS_PERM_MASK        0x0F
#define FS_MODE_READ        0x1
#define FS_MODE_WRITE       0x2
#define FS_MODE_EXEC        0x4

#define FS_INVALID_BLOCK    0xFFFFFFFF
#define FS_INVALID_INODE    0xFFFFFFFF

// FileSystem memory location
#define FS_MEMORY 0x1000000

typedef struct superblock {
    uint32_t magic;
    uint32_t block_size;
    uint32_t max_blocks;
    uint32_t max_inodes;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t inode_table_start;
    uint32_t data_block_start;
} superblock;

typedef struct inode {
    uint32_t number;
    uint32_t size;
    uint16_t mode;           // type + permissions
    uint16_t links_count;
    uint32_t block_pointers[10];
    uint32_t indirect_block;
    uint32_t double_indirect;
} inode;

typedef struct dir_entry {
    uint32_t inode;             // inode number of the file/folder
    char name[FS_MAX_FILENAME+1]; // filename
} dir_entry;
