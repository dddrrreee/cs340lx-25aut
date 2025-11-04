#ifndef __MY_FAT32_H__
#define __MY_FAT32_H__

#include "rpi.h"
#include "fat32.h"

typedef struct {
    fat32_fs_t fs;
    pi_dirent_t root;
} my_fat32_t;

static my_fat32_t my_fat32;
static int my_fat32_initialized = 0;

static void inline my_fat32_init() {
    if (my_fat32_initialized) {
        return;
    }

    kmalloc_init(FAT32_HEAP_MB);
    pi_sd_init();
  
    // printk("Reading the MBR.\n");
    mbr_t *mbr = mbr_read();
  
    // printk("Loading the first partition.\n");
    mbr_partition_ent_t partition;
    memcpy(&partition, mbr->part_tab1, sizeof(mbr_partition_ent_t));
    assert(mbr_part_is_fat32(partition.part_type));
  
    // printk("Loading the FAT.\n");
    my_fat32.fs = fat32_mk(&partition);
  
    // printk("Loading the root directory.\n");
    my_fat32.root = fat32_get_root(&my_fat32.fs);

    my_fat32_initialized = 1;
}

// This is very unique to our Pi setup. `buffer` points to a physical address.
// Neither the caller or this function allocates anything. It just writes to
// the physical address pointed to by `buffer`, and return the number of bytes written.
static int inline my_fat32_read(char *name, char *buffer) {
    if (!my_fat32_initialized) {
        my_fat32_init();
    }

    pi_file_t *file = fat32_read(&my_fat32.fs, &my_fat32.root, name);

    if (!file) {
        printk("%s not found.\n", name);
        return -1;
    }

    memcpy(buffer, file->data, file->n_data);
    return file->n_data;
}

#endif
