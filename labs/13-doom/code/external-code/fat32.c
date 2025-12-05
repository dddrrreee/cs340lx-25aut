#include "rpi.h"
#include "fat32.h"
#include "fat32-helpers.h"
#include "pi-sd.h"

// Print extra tracing info when this is enabled.  You can and should add your
// own.
static int trace_p = 0; 
static int init_p = 0;

fat32_boot_sec_t boot_sector;


fat32_fs_t fat32_mk(mbr_partition_ent_t *partition) {
  demand(!init_p, "the fat32 module is already in use\n");
  // TODO: Read the boot sector (of the partition) off the SD card.
  pi_sd_read(&boot_sector, partition->lba_start, 1);

  // TODO: Verify the boot sector (also called the volume id, `fat32_volume_id_check`)
  fat32_volume_id_check(&boot_sector);
  // fat32_volume_id_print("volume id/boot sector", &boot_sector);

  // TODO: Read the FS info sector (the sector immediately following the boot
  // sector) and check it (`fat32_fsinfo_check`, `fat32_fsinfo_print`)
  assert(boot_sector.info_sec_num == 1);
  struct fsinfo fsinfo_sector;
  pi_sd_read(&fsinfo_sector, partition->lba_start + 1, 1);

  fat32_fsinfo_check(&fsinfo_sector);
  // fat32_fsinfo_print("filesystem info sector", &fsinfo_sector);

  // END OF PART 2
  // The rest of this is for Part 3:

  // TODO: calculate the fat32_fs_t metadata, which we'll need to return.
  unsigned lba_start = partition->lba_start; // from the partition
  unsigned fat_begin_lba = lba_start + boot_sector.reserved_area_nsec; // the start LBA + the number of reserved sectors
  unsigned cluster_begin_lba = fat_begin_lba + (boot_sector.nfats * boot_sector.nsec_per_fat); // the beginning of the FAT, plus the combined length of all the FATs
  unsigned sec_per_cluster = boot_sector.sec_per_cluster; // from the boot sector
  unsigned root_first_cluster = boot_sector.first_cluster; // from the boot sector
  unsigned n_entries = boot_sector.nsec_per_fat * boot_sector.bytes_per_sec / sizeof(uint32_t); // from the boot sector

  /*
   * TODO: Read in the entire fat (one copy: worth reading in the second and
   * comparing).
   *
   * The disk is divided into clusters. The number of sectors per
   * cluster is given in the boot sector byte 13. <sec_per_cluster>
   *
   * The File Allocation Table has one entry per cluster. This entry
   * uses 12, 16 or 28 bits for FAT12, FAT16 and FAT32.
   *
   * Store the FAT in a heap-allocated array.
   */
  uint32_t *fat;
  fat = pi_sec_read(fat_begin_lba, boot_sector.nsec_per_fat);

  // Create the FAT32 FS struct with all the metadata
  fat32_fs_t fs = (fat32_fs_t) {
    .lba_start = lba_start,
      .fat_begin_lba = fat_begin_lba,
      .cluster_begin_lba = cluster_begin_lba,
      .sectors_per_cluster = sec_per_cluster,
      .root_dir_first_cluster = root_first_cluster,
      .fat = fat,
      .n_entries = n_entries,
  };

  if (trace_p) {
    trace("begin lba = %d\n", fs.fat_begin_lba);
    trace("cluster begin lba = %d\n", fs.cluster_begin_lba);
    trace("sectors per cluster = %d\n", fs.sectors_per_cluster);
    trace("root dir first cluster = %d\n", fs.root_dir_first_cluster);
  }

  init_p = 1;
  return fs;
}

// Given cluster_number, get lba.  Helper function.
static uint32_t cluster_to_lba(fat32_fs_t *f, uint32_t cluster_num) {
  assert(cluster_num >= 2);
  // TODO: calculate LBA from cluster number, cluster_begin_lba, and
  // sectors_per_cluster
  unsigned lba = f->cluster_begin_lba + (cluster_num - 2) * f->sectors_per_cluster;
  if (trace_p) trace("cluster %d to lba: %d\n", cluster_num, lba);
  return lba;
}

pi_dirent_t fat32_get_root(fat32_fs_t *fs) {
  demand(init_p, "fat32 not initialized!");
  // TODO: return the information corresponding to the root directory (just
  // cluster_id, in this case)
  return (pi_dirent_t) {
    .name = "",
      .raw_name = "",
      .cluster_id = fs->root_dir_first_cluster, // fix this
      .is_dir_p = 1,
      .nbytes = 0,
  };
}

// Given the starting cluster index, get the length of the chain.  Helper
// function.
static uint32_t get_cluster_chain_length(fat32_fs_t *fs, uint32_t start_cluster) {
  // TODO: Walk the cluster chain in the FAT until you see a cluster where
  // `fat32_fat_entry_type(cluster) == LAST_CLUSTER`.  Count the number of
  // clusters.
  uint32_t count = 0;
  if (start_cluster == 0) {
    return 0;
  }

  while (fat32_fat_entry_type(start_cluster) != LAST_CLUSTER) {
    start_cluster = (start_cluster << 4) >> 4;
    start_cluster = *(fs->fat + start_cluster);

    count++;
  }

  return count;
}

// Given the starting cluster index, read a cluster chain into a contiguous
// buffer.  Assume the provided buffer is large enough for the whole chain.
// Helper function.
static void read_cluster_chain(fat32_fs_t *fs, uint32_t start_cluster, uint8_t *data) {
  // TODO: Walk the cluster chain in the FAT until you see a cluster where
  // fat32_fat_entry_type(cluster) == LAST_CLUSTER.  For each cluster, copy it
  // to the buffer (`data`).  Be sure to offset your data pointer by the
  // appropriate amount each time.
  uint32_t bytes_in_cluster = fs->sectors_per_cluster * 512;

  while (fat32_fat_entry_type(start_cluster) != LAST_CLUSTER) {
    start_cluster = (start_cluster << 4) >> 4;

    pi_sd_read(data, cluster_to_lba(fs, start_cluster), fs->sectors_per_cluster);
    data += bytes_in_cluster;

    start_cluster = *(fs->fat + start_cluster);
  }
}

// Converts a fat32 internal dirent into a generic one suitable for use outside
// this driver.
static pi_dirent_t dirent_convert(fat32_dirent_t *d) {
  pi_dirent_t e = {
    .cluster_id = fat32_cluster_id(d),
    .is_dir_p = d->attr == FAT32_DIR,
    .nbytes = d->file_nbytes,
  };
  // can compare this name
  memcpy(e.raw_name, d->filename, sizeof d->filename);
  // for printing.
  fat32_dirent_name(d,e.name);
  return e;
}

// Gets all the dirents of a directory which starts at cluster `cluster_start`.
// Return a heap-allocated array of dirents.
static fat32_dirent_t *get_dirents(fat32_fs_t *fs, uint32_t cluster_start, uint32_t *dir_n) {
  // TODO: figure out the length of the cluster chain (see
  // `get_cluster_chain_length`)
  uint32_t num_clusters = get_cluster_chain_length(fs, cluster_start);

  // TODO: allocate a buffer large enough to hold the whole directory
  uint8_t *buf = kmalloc(num_clusters * fs->sectors_per_cluster * 512); 
  *dir_n = num_clusters * fs->sectors_per_cluster * 512 / sizeof(fat32_dirent_t);

  // TODO: read in the whole directory (see `read_cluster_chain`)
  read_cluster_chain(fs, cluster_start, buf);
  return (fat32_dirent_t *) buf;
}

pi_directory_t fat32_readdir(fat32_fs_t *fs, pi_dirent_t *dirent) {
  demand(init_p, "fat32 not initialized!");
  demand(dirent->is_dir_p, "tried to readdir a file!");
  // TODO: use `get_dirents` to read the raw dirent structures from the disk
  uint32_t n_dirents;
  fat32_dirent_t *dirents = get_dirents(fs, dirent->cluster_id, &n_dirents);

  // TODO: allocate space to store the pi_dirent_t return values
  pi_dirent_t *valid_dirents = kmalloc(n_dirents * sizeof(pi_dirent_t));

  // TODO: iterate over the directory and create pi_dirent_ts for every valid
  // file.  Don't include empty dirents, LFNs, or Volume IDs.  You can use
  // `dirent_convert
  uint32_t n_valid_dirents = 0;

  for (uint32_t i = 0; i < n_dirents; i++) {
    // long file name
    if ((dirents + i)->attr == FAT32_LONG_FILE_NAME) {
      continue;
    } else if (((dirents + i)->attr & FAT32_VOLUME_LABEL) == FAT32_VOLUME_LABEL) {
      continue;
    } else if (fat32_dirent_free(dirents + i)) {
      continue;
    } else if ((dirents + i)->filename[0] == 0) {
      break;
    } else {
      *(valid_dirents + n_valid_dirents) = dirent_convert(dirents + i);
      n_valid_dirents++;
    }
  }

  // TODO: create a pi_directory_t using the dirents and the number of valid
  // dirents we found
  return (pi_directory_t) {
    .dirents = valid_dirents,
    .ndirents = n_valid_dirents,
  };
}

static int find_dirent_with_name(fat32_dirent_t *dirents, int n, char *filename) {
  // TODO: iterate through the dirents, looking for a file which matches the
  // name; use `fat32_dirent_name` to convert the internal name format to a
  // normal string.
  int fname_len = strlen(filename);

  for (int i=0; i < n; i++) {
    char name[13];
    fat32_dirent_name(dirents + i, name);

    int name_len = strlen(name);

    if (name_len == fname_len) {
      if (strncmp(name, filename, fname_len) == 0) {
        return i;
      }
    }
  }
  
  return -1;
}

pi_dirent_t *fat32_stat(fat32_fs_t *fs, pi_dirent_t *directory, char *filename) {
  demand(init_p, "fat32 not initialized!");
  demand(directory->is_dir_p, "tried to use a file as a directory");

  // TODO: use `get_dirents` to read the raw dirent structures from the disk
  uint32_t dir_n;
  fat32_dirent_t * raw_dirents = get_dirents(fs, directory->cluster_id, &dir_n);

  // TODO: Iterate through the directory's entries and find a dirent with the
  // provided name.  Return NULL if no such dirent exists.  You can use
  // `find_dirent_with_name` if you've implemented it.
  int ind = find_dirent_with_name(raw_dirents, dir_n, filename);

  if (ind == -1) {
    return NULL;
  }

  // TODO: allocate enough space for the dirent, then convert
  // (`dirent_convert`) the fat32 dirent into a Pi dirent.
  pi_dirent_t *dirent = kmalloc(sizeof(pi_dirent_t));
  *dirent = dirent_convert(raw_dirents + ind);

  return dirent;
}

pi_file_t *fat32_read(fat32_fs_t *fs, pi_dirent_t *directory, char *filename) {
  // This should be pretty similar to readdir, but simpler.
  demand(init_p, "fat32 not initialized!");
  demand(directory->is_dir_p, "tried to use a file as a directory!");

  // TODO: read the dirents of the provided directory and look for one matching the provided name
  pi_dirent_t *dirent = fat32_stat(fs, directory, filename);

  // TODO: figure out the length of the cluster chain
  uint32_t chain_length = get_cluster_chain_length(fs, dirent->cluster_id);

  // TODO: allocate a buffer large enough to hold the whole file
  size_t bytes_alloc = chain_length * fs->sectors_per_cluster * 512;
  uint8_t *buf = kmalloc(bytes_alloc);

  // TODO: read in the whole file (if it's not empty)
  read_cluster_chain(fs, dirent->cluster_id, buf);

  // TODO: fill the pi_file_t
  pi_file_t *file = kmalloc(sizeof(pi_file_t));
  *file = (pi_file_t) {
    .data = buf,
    .n_data = dirent->nbytes,
    .n_alloc = bytes_alloc,
  };
  return file;
}

/******************************************************************************
 * Everything below here is for writing to the SD card (Part 7/Extension).  If
 * you're working on read-only code, you don't need any of this.
 ******************************************************************************/

static uint32_t find_free_cluster(fat32_fs_t *fs, uint32_t start_cluster) {
  // TODO: loop through the entries in the FAT until you find a free one
  // (fat32_fat_entry_type == FREE_CLUSTER).  Start from cluster 3.  Panic if
  // there are none left.
  if (start_cluster < 3) start_cluster = 3;
  
  for (uint32_t i = start_cluster; i < fs->n_entries; i++) {
    if (fat32_fat_entry_type(*(fs->fat + i)) == FREE_CLUSTER) {
      return i;
    }
  }

  if (trace_p) trace("failed to find free cluster from %d\n", start_cluster);
  panic("No more clusters on the disk!\n");
}

static void write_fat_to_disk(fat32_fs_t *fs) {
  // TODO: Write the FAT to disk.  In theory we should update every copy of the
  // FAT, but the first one is probably good enough.  A good OS would warn you
  // if the FATs are out of sync, but most OSes just read the first one without
  // complaining.
  if (trace_p) trace("syncing FAT\n");

  pi_sd_write(fs->fat, fs->fat_begin_lba, boot_sector.nsec_per_fat);
}

// Given the starting cluster index, write the data in `data` over the
// pre-existing chain, adding new clusters to the end if necessary.
static void write_cluster_chain(fat32_fs_t *fs, uint32_t start_cluster, uint8_t *data, uint32_t nbytes) {
  // Walk the cluster chain in the FAT, writing the in-memory data to the
  // referenced clusters.  If the data is longer than the cluster chain, find
  // new free clusters and add them to the end of the list.
  // Things to consider:
  //  - what if the data is shorter than the cluster chain?
  //  - what if the data is longer than the cluster chain?
  //  - the last cluster needs to be marked LAST_CLUSTER
  //  - when do we want to write the updated FAT to the disk to prevent
  //  corruption?
  //  - what do we do when nbytes is 0?
  //  - what about when we don't have a valid cluster to start with?
  //
  //  This is the main "write" function we'll be using; the other functions
  //  will delegate their writing operations to this.
  uint32_t chain_length = get_cluster_chain_length(fs, start_cluster);
  int n_bytes = nbytes;

  printk("write cluster chain function:\n");
  printk("chain_length = %d, start_cluster = %d, n_bytes = %d\n", chain_length, start_cluster, n_bytes);

  // TODO: As long as we have bytes left to write and clusters to write them
  // to, walk the cluster chain writing them out.
  uint32_t prev_cluster = 0;

  while (n_bytes > 0 && chain_length > 0) {
    pi_sd_write(data, cluster_to_lba(fs, start_cluster), fs->sectors_per_cluster);
    chain_length--;
    n_bytes -= fs->sectors_per_cluster * 512;
    data += fs->sectors_per_cluster * 512;

    prev_cluster = start_cluster;
    start_cluster = *(fs->fat + start_cluster);
    start_cluster = (start_cluster << 4) >> 4;
  }

  printk("prev_cluster = %d\n", prev_cluster);
  printk("chain_length = %d, start_cluster = %d, n_bytes = %d\n", chain_length, start_cluster, n_bytes);

  // TODO: If we run out of clusters to write to, find free clusters using the
  // FAT and continue writing the bytes out.  Update the FAT to reflect the new
  // cluster.
  if (n_bytes > 0 && chain_length == 0) {
    printk("case 1:\n");
    while (n_bytes > 0) {
      uint32_t free_cluster = find_free_cluster(fs, 3);
      pi_sd_write(data, cluster_to_lba(fs, free_cluster), fs->sectors_per_cluster);
      n_bytes -= fs->sectors_per_cluster * 512;
      data += fs->sectors_per_cluster * 512;

      // update fat to have link between prev_cluster and free_cluster
      *(fs->fat + prev_cluster) = free_cluster;
      printk("prev_cluster = %d, free_cluster = %d\n", prev_cluster, free_cluster);
      printk("n_bytes = %d\n", n_bytes);
      prev_cluster = free_cluster;
    }

    // prev_cluster should have the last cluster at the end of this
    *(fs->fat + prev_cluster) = 0xffffffff;
  } 

  // TODO: If we run out of bytes to write before using all the clusters, mark
  // the final cluster as "LAST_CLUSTER" in the FAT, then free all the clusters
  // later in the chain.
  if (n_bytes <= 0 && chain_length > 0) {
    printk("case 2:\n");
    uint32_t free_chain_start = 0;

    if (prev_cluster == 0) {
      free_chain_start = *(fs->fat + start_cluster);
    } else {
      free_chain_start = *(fs->fat + prev_cluster);
    }

    free_chain_start = (free_chain_start << 4) >> 4;

    while (fat32_fat_entry_type(free_chain_start) != LAST_CLUSTER) {
      uint32_t cluster_id = (free_chain_start << 4) >> 4;
      free_chain_start = *(fs->fat + cluster_id);

      assert(cluster_id < fs->n_entries);
      printk("freeing cluster %d\n", cluster_id);
      *(fs->fat + cluster_id) = 0;
    }

    // prev_cluster should have the last cluster at the end of this
    if (prev_cluster != 0) {
      *(fs->fat + prev_cluster) = 0xffffffff;
    }
    printk("last cluster: %d\n", prev_cluster);
  }

  // TODO: Ensure that the last cluster in the chain is marked "LAST_CLUSTER".
  // The one exception to this is if we're writing 0 bytes in total, in which
  // case we don't want to use any clusters at all.
  if (prev_cluster != 0) {
    assert(fat32_fat_entry_type(*(fs->fat + prev_cluster)) == LAST_CLUSTER);
  }
}

int fat32_rename(fat32_fs_t *fs, pi_dirent_t *directory, char *oldname, char *newname) {
  // TODO: Get the dirents `directory` off the disk, and iterate through them
  // looking for the file.  When you find it, rename it and write it back to
  // the disk (validate the name first).  Return 0 in case of an error, or 1
  // on success.
  // Consider:
  //  - what do you do when there's already a file with the new name?
  demand(init_p, "fat32 not initialized!");
  if (trace_p) trace("renaming %s to %s\n", oldname, newname);
  if (!fat32_is_valid_name(newname)) return 0;

  uint32_t dir_n;
  fat32_dirent_t *raw_dirents = get_dirents(fs, directory->cluster_id, &dir_n);

  int ind = find_dirent_with_name(raw_dirents, dir_n, oldname);

  if (ind == -1) {
    trace("could not find file\n");
    return 0;
  }

  // TODO: update the dirent's name
  fat32_dirent_set_name(raw_dirents + ind, newname);

  // TODO: write out the directory, using the existing cluster chain (or
  // appending to the end); implementing `write_cluster_chain` will help
  write_cluster_chain(fs, directory->cluster_id, (uint8_t *) raw_dirents, dir_n * sizeof(fat32_dirent_t));
  return 1;

}

// Create a new directory entry for an empty file (or directory).
pi_dirent_t *fat32_create(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, int is_dir) {
  demand(init_p, "fat32 not initialized!");
  if (trace_p) trace("creating %s\n", filename);
  if (!fat32_is_valid_name(filename)) {
    trace("invalid filename\n");
    return NULL;
  }

  // TODO: read the dirents and make sure there isn't already a file with the
  // same name
  uint32_t dir_n;
  fat32_dirent_t *raw_dirents = get_dirents(fs, directory->cluster_id, &dir_n);

  int ind = find_dirent_with_name(raw_dirents, dir_n, filename);

  if (ind != -1) {
    trace("file with same name already exists\n");
    return NULL;
  }

  // TODO: look for a free directory entry and use it to store the data for the
  // new file.  If there aren't any free directory entries, either panic or
  // (better) handle it appropriately by extending the directory to a new
  // cluster.
  // When you find one, update it to match the name and attributes
  // specified; set the size and cluster to 0.
  uint32_t free_dir_ind = 0;
  uint32_t found_free_dir = 0;

  for (; free_dir_ind < dir_n; free_dir_ind++) {
    if (fat32_dirent_free(raw_dirents + free_dir_ind)) {
      found_free_dir = 1;
      break;
    }
  }

  if (found_free_dir) {
    // set filename of new dirent
    fat32_dirent_set_name(raw_dirents + free_dir_ind, filename);

    // set start cluster to 0
    (raw_dirents + free_dir_ind)->hi_start = 0;
    (raw_dirents + free_dir_ind)->lo_start = 0;

    // set file size to 0
    (raw_dirents + free_dir_ind)->file_nbytes = 0;

    // set dirent attribute
    if (is_dir) {
      (raw_dirents + free_dir_ind)->attr = FAT32_DIR;
    } else {
      (raw_dirents + free_dir_ind)->attr = 0;
    }
  } else {
    panic("fat32_create: could not find empty dirent for new file\n");
  }

  // TODO: write out the updated directory to the disk
  write_cluster_chain(fs, directory->cluster_id, (uint8_t *) raw_dirents, dir_n * sizeof(fat32_dirent_t));

  // TODO: convert the dirent to a `pi_dirent_t` and return a (kmalloc'ed)
  // pointer
  pi_dirent_t *dirent = kmalloc(sizeof(pi_dirent_t));
  *dirent = dirent_convert(raw_dirents + free_dir_ind);

  return dirent;
}

// Delete a file, including its directory entry.
int fat32_delete(fat32_fs_t *fs, pi_dirent_t *directory, char *filename) {
  demand(init_p, "fat32 not initialized!");
  if (trace_p) trace("deleting %s\n", filename);
  if (!fat32_is_valid_name(filename)) return 0;
  // TODO: look for a matching directory entry, and set the first byte of the
  // name to 0xE5 to mark it as free
  uint32_t dir_n;
  fat32_dirent_t *raw_dirents = get_dirents(fs, directory->cluster_id, &dir_n);

  int ind = find_dirent_with_name(raw_dirents, dir_n, filename);

  if (ind == -1) {
    trace("file does not exist, cannot delete\n");
    return 0;
  } else {
    trace("file exists\n");
  }

  uint32_t start_cluster = fat32_cluster_id(raw_dirents + ind);
  printk("start_cluster = %d\n", start_cluster);

  *((raw_dirents + ind)->filename) = 0xe5; // mark dirent as free
  assert(fat32_dirent_free(raw_dirents + ind));

  // TODO: free the clusters referenced by this dirent
  if (start_cluster != 0) {
    while (fat32_fat_entry_type(start_cluster) != LAST_CLUSTER) {
      uint32_t cluster_id = (start_cluster << 4) >> 4;
      start_cluster = *(fs->fat + cluster_id);
  
      assert(cluster_id < fs->n_entries);
      *(fs->fat + cluster_id) = 0;
    }
  } else {
    printk("file had no allocated clusters\n");
  }

  // TODO: write out the updated directory to the disk
  write_fat_to_disk(fs);
  write_cluster_chain(fs, directory->cluster_id, (uint8_t *) raw_dirents, dir_n * sizeof(fat32_dirent_t));

  return 1;
}

// don't need this function
int fat32_truncate(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, unsigned length) {
  demand(init_p, "fat32 not initialized!");
  if (trace_p) trace("truncating %s\n", filename);

  // TODO: edit the directory entry of the file to list its length as `length` bytes,
  // then modify the cluster chain to either free unused clusters or add new
  // clusters.
  //
  // Consider: what if the file we're truncating has length 0? what if we're
  // truncating to length 0?
  unimplemented();

  // TODO: write out the directory entry
  unimplemented();
  return 0;
}

int fat32_write(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, pi_file_t *file) {
  demand(init_p, "fat32 not initialized!");
  demand(directory->is_dir_p, "tried to use a file as a directory!");

  // TODO: Surprisingly, this one should be rather straightforward now.
  // - load the directory
  // - exit with an error (0) if there's no matching directory entry
  // - update the directory entry with the new size
  // - write out the file as clusters & update the FAT
  // - write out the directory entry
  // Special case: the file is empty to start with, so we need to update the
  // start cluster in the dirent

  trace("start of write function\n");

  uint32_t dir_n;
  fat32_dirent_t *raw_dirents = get_dirents(fs, directory->cluster_id, &dir_n);

  int ind = find_dirent_with_name(raw_dirents, dir_n, filename);

  if (ind == -1) {
    trace("found no dirents with that filename\n");
    return 0;
  } else {
    trace("file with name %s exists\n", filename);
  }

  (raw_dirents + ind)->file_nbytes = file->n_data;
  uint32_t cur_cluster = fat32_cluster_id(raw_dirents + ind);

  uint32_t start_cluster = 0;

  if (cur_cluster == 0) {
    start_cluster = find_free_cluster(fs, 3);
    *(fs->fat + start_cluster) = 0xffffffff;

    (raw_dirents + ind)->hi_start = start_cluster >> 16;
    (raw_dirents + ind)->lo_start = start_cluster & ((1 << 16) - 1);

    assert(get_cluster_chain_length(fs, start_cluster) == 1);
  } else {
    start_cluster = cur_cluster;
  }

  // check if no bytes are written in the file
  if (file->n_data == 0) {
    (raw_dirents + ind)->hi_start = 0;
    (raw_dirents + ind)->lo_start = 0;

    *(fs->fat + start_cluster) = 0;
  }

  // write out actual file data and update fat on disk
  write_cluster_chain(fs, start_cluster, file->data, file->n_data);
  write_fat_to_disk(fs);

  // write updated dirent to disk
  write_cluster_chain(fs, directory->cluster_id, (uint8_t *) raw_dirents, dir_n * sizeof(fat32_dirent_t));

  return 1;
}

int fat32_flush(fat32_fs_t *fs) {
  demand(init_p, "fat32 not initialized!");
  // no-op
  return 0;
}
