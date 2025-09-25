#include "fs.h"
#include "../../stdlib/stdio.h"
#include "../../vga.h"
#include "../../util.h"
#include "../../string.h"

fs_t fs;

void fs_init(const uint8_t *disk_image) {
    fs.disk_data = disk_image;
    fs.bpb = (const fat_bpb_t *)disk_image;

    uint16_t bytes_per_sector   = fs.bpb->bytes_per_sector;
    uint8_t  sectors_per_cluster = fs.bpb->sectors_per_cluster;
    uint16_t reserved_sectors   = fs.bpb->reserved_sectors;
    uint8_t  fat_count          = fs.bpb->fat_count;

    uint32_t sectors_per_fat = fs.bpb->sectors_per_fat16 ?
                               fs.bpb->sectors_per_fat16 :
                               fs.bpb->sectors_per_fat32;

    fs.fat_start      = reserved_sectors * bytes_per_sector;
    fs.root_dir_start = fs.fat_start + (fat_count * sectors_per_fat * bytes_per_sector);
    fs.data_start     = fs.root_dir_start +
                        (fs.bpb->root_entries * 32); // 32 bytes per root entry

    print("FS initialized\n");
    fs_print_info();
}

void fs_read_cluster(uint32_t cluster, void *buffer) {
    uint32_t cluster_size = fs.bpb->sectors_per_cluster * fs.bpb->bytes_per_sector;
    uint32_t offset = fs.data_start + (cluster - 2) * cluster_size;

    const uint8_t *src = fs.disk_data + offset;
    uint8_t *dst = (uint8_t*)buffer;

    for (uint32_t i = 0; i < cluster_size; i++) {
        dst[i] = src[i];
    }
}

void fs_print_info() {
    print("=== FAT Filesystem Info ===\n");
    print("Bytes per sector: "); print_dec(fs.bpb->bytes_per_sector); print("\n");
    print("Sectors per cluster: "); print_dec(fs.bpb->sectors_per_cluster); print("\n");
    print("Reserved sectors: "); print_dec(fs.bpb->reserved_sectors); print("\n");
    print("FATs: "); print_dec(fs.bpb->fat_count); print("\n");
    print("Root entries: "); print_dec(fs.bpb->root_entries); print("\n");
    print("Total sectors16: "); print_dec(fs.bpb->total_sectors16); print("\n");
    print("Total sectors32: "); print_dec(fs.bpb->total_sectors32); print("\n");
    print("Sectors per FAT16: "); print_dec(fs.bpb->sectors_per_fat16); print("\n");
    print("Sectors per FAT32: "); print_dec(fs.bpb->sectors_per_fat32); print("\n");

    print("FAT start offset: "); print_hex32(fs.fat_start); print("\n");
    print("Root dir offset: "); print_hex32(fs.root_dir_start); print("\n");
    print("Data region offset: "); print_hex32(fs.data_start); print("\n");
}

void fs_write_cluster(uint32_t cluster, const void *buffer) {
    uint32_t cluster_size = fs.bpb->sectors_per_cluster * fs.bpb->bytes_per_sector;
    uint32_t offset = fs.data_start + (cluster - 2) * cluster_size;

    const uint8_t *src = (const uint8_t*)buffer;
    uint8_t *dst = (uint8_t*)fs.disk_data + offset;

    for (uint32_t i = 0; i < cluster_size; i++) {
        dst[i] = src[i];
    }
}

fat_dir_entry_t* fs_find_file(const char* name) {
    uint32_t root_cluster = fs.bpb->root_cluster;
    uint8_t cluster_buf[MAX_CLUSTER_SIZE];
    fs_read_cluster(root_cluster, cluster_buf);

    fat_dir_entry_t* entries = (fat_dir_entry_t*)cluster_buf;
    int num_entries = (fs.bpb->sectors_per_cluster * fs.bpb->bytes_per_sector) / sizeof(fat_dir_entry_t);

    for (int i = 0; i < num_entries; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;

        char fname[13];
        fs_get_filename(&entries[i], fname);
        if (strcmp(fname, name) == 0) {
            return &entries[i];
        }
    }
    return NULL;
}

void fs_get_filename(fat_dir_entry_t* entry, char* out_name) {
    int i, j = 0;

    // Copy name (first 8 bytes)
    for (i = 0; i < 8 && entry->name[i] != ' '; i++) {
        out_name[j++] = entry->name[i];
    }

    // Copy extension (last 3 bytes)
    int has_ext = 0;
    for (i = 8; i < 11; i++) {
        if (entry->name[i] != ' ') has_ext = 1;
    }

    if (has_ext) {
        out_name[j++] = '.';
        for (i = 8; i < 11 && entry->name[i] != ' '; i++) {
            out_name[j++] = entry->name[i];
        }
    }

    out_name[j] = '\0';
}

uint32_t fs_next_cluster(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4; // FAT32 entries are 4 bytes
    uint32_t fat_entry = *(uint32_t*)(fs.disk_data + fs.fat_start + fat_offset);
    return fat_entry & 0x0FFFFFFF;     // mask high 4 bits
}

void fs_list_root(void) {
    uint32_t root_cluster = fs.bpb->root_cluster;
    uint32_t cluster_size = fs.bpb->sectors_per_cluster * fs.bpb->bytes_per_sector;

    uint8_t buffer[MAX_CLUSTER_SIZE];
    fs_read_cluster(root_cluster, buffer);

    const fat_dir_entry_t *entry = (const fat_dir_entry_t *)buffer;

    print("=== Root Directory Listing ===\n");

    for (uint32_t i = 0; i < cluster_size / sizeof(fat_dir_entry_t); i++, entry++) {
        if (entry->name[0] == 0x00) break;           // end of entries
        if ((uint8_t)entry->name[0] == 0xE5) continue; // deleted entry
        if (entry->attr & ATTR_VOLUME_ID) continue;    // skip volume label

        char name[13];
        int j = 0;

        // Copy base name
        for (int k = 0; k < 8 && entry->name[k] != ' '; k++) {
            name[j++] = entry->name[k];
        }

        // Add extension if present
        if (entry->name[8] != ' ') {
            name[j++] = '.';
            for (int k = 8; k < 11 && entry->name[k] != ' '; k++) {
                name[j++] = entry->name[k];
            }
        }

        name[j] = '\0';

        print(name);
        if (entry->attr & ATTR_DIRECTORY) {
            print(" <DIR>");
        }
        print("\n");
    }
}

void fs_create_file(const char *name) {
    // Load root directory cluster
    uint32_t cluster_size = fs.bpb->sectors_per_cluster * fs.bpb->bytes_per_sector;
    uint8_t buffer[4096]; // should be enough for one cluster
    fs_read_cluster(fs.bpb->root_cluster, buffer);

    // Find free entry
    fat_dir_entry_t *entries = (fat_dir_entry_t*)buffer;
    int max_entries = cluster_size / sizeof(fat_dir_entry_t);

    for (int i = 0; i < max_entries; i++) {
        if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5) {
            // Found free entry
            memset(entries[i].name, ' ', 11);
            int j = 0;
            // Write up to 8 chars for name
            for (; j < 8 && name[j] && name[j] != '.'; j++) {
                entries[i].name[j] = toupper(name[j]);
            }
            // If extension
            const char *ext = strchr(name, '.');
            if (ext) {
                ext++;
                for (int k = 0; k < 3 && ext[k]; k++) {
                    entries[i].name[8+k] = toupper(ext[k]);
                }
            }

            entries[i].attr = 0x20; // Archive
            entries[i].first_cluster_high = 0;
            entries[i].first_cluster_low = 2; // start at cluster 2 for now
            entries[i].file_size = 0;

            // Write cluster back
            fs_write_cluster(fs.bpb->root_cluster, buffer);
            print("Created file: "); print(name); print("\n");
            return;
        }
    }

    print("No free directory entry found!\n");
}

void fs_cat_file(const char* name) {
    fat_dir_entry_t* entry = fs_find_file(name);
    if (!entry) {
        print("File not found.\n");
        return;
    }

    uint32_t cluster = (entry->first_cluster_high << 16) | entry->first_cluster_low;
    uint32_t size = entry->file_size;
    uint8_t buffer[fs.bpb->bytes_per_sector * fs.bpb->sectors_per_cluster];

    while (size > 0) {
        fs_read_cluster(cluster, buffer);
        uint32_t to_print = size > sizeof(buffer) ? sizeof(buffer) : size;
        for (uint32_t i = 0; i < to_print; i++)
            putc(buffer[i]);
        size -= to_print;

        cluster = fs_next_cluster(cluster); // function that reads FAT table
        if (cluster == 0x0FFFFFF8) break;    // end of file
    }
}
