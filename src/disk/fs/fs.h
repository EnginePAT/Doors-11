#ifndef FS_H
#define FS_H

#include <stdint.h>

#define ATTR_DIRECTORY 0x10
#define ATTR_VOLUME_ID 0x08
#define MAX_CLUSTER_SIZE 4096

// BIOS Parameter Block (BPB) for FAT
typedef struct {
    uint8_t  jmp[3];          // Jump instruction
    uint8_t  oem[8];          // OEM string
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint16_t root_entries;
    uint16_t total_sectors16;
    uint8_t  media_descriptor;
    uint16_t sectors_per_fat16;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors32;

    // FAT32 extensions
    uint32_t sectors_per_fat32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
} __attribute__((packed)) fat_bpb_t;

typedef struct {
    char     name[11];   // 8.3 format (no dot)
    uint8_t  attr;
    uint8_t  ntres;
    uint8_t  crt_time_tenth;
    uint16_t crt_time;
    uint16_t crt_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t wrt_time;
    uint16_t wrt_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed)) fat_dir_entry_t;

// FS state
typedef struct {
    const uint8_t *disk_data; // pointer to loaded module (FAT image)
    const fat_bpb_t *bpb;     // pointer to BPB in boot sector
    uint32_t fat_start;       // FAT offset (in bytes)
    uint32_t root_dir_start;  // root directory offset (in bytes)
    uint32_t data_start;      // data region offset (in bytes)
} fs_t;

// Global FS
extern fs_t fs;

// Initialize filesystem
void fs_init(const uint8_t *disk_image);

// Read a cluster into buffer
void fs_read_cluster(uint32_t cluster, void *buffer);

// Print basic FS info
void fs_print_info();

// Print the contents of the root directory
void fs_list_root(void); 

void fs_get_filename(fat_dir_entry_t* entry, char* out_name);
uint32_t fs_next_cluster(uint32_t cluster);

void fs_write_cluster(uint32_t cluster, const void *buffer);
void fs_create_file(const char *name);
void fs_cat_file(const char* name);

#endif
