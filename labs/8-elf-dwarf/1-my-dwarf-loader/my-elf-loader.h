#ifndef __MY_ELF_H__
#define __MY_ELF_H__

#include "rpi.h"
#include "include/my-fat32-driver.h"

/*
     <ELF Format>

    ========================
    | ELF Header           |
    ========================
    | Program Header Table |
    ========================
    | Section 1            |
    ========================
    | Section 2            |
    ========================
    | ...                  |
    ========================
    | Section n            |
    ========================
    | Section Header Table |
    ========================
*/

// ELF Header
#define E_NIDENT 16
typedef struct {
    uint8_t     e_ident[E_NIDENT];
    uint16_t    e_type;
    uint16_t    e_machine;
    uint32_t    e_version;
    uint32_t    e_entry;
    uint32_t    e_phoff;
    uint32_t    e_shoff;
    uint32_t    e_flags;
    uint16_t    e_ehsize;
    uint16_t    e_phentsize;
    uint16_t    e_phnum;
    uint16_t    e_shentsize;
    uint16_t    e_shnum;
    uint16_t    e_shstrndx;
} elf32_header; // 52 bytes (0x34)

// Program Header
typedef struct {
    uint32_t    p_type;
    uint32_t    p_offset; 
    uint32_t    p_vaddr; 
    uint32_t    p_paddr; 
    uint32_t    p_filesz; 
    uint32_t    p_memsz; 
    uint32_t    p_flags; 
    uint32_t    p_align;
} elf32_pheader; // 32 bytes (0x20)

// Section Header
typedef struct {
    uint32_t    sh_name; 
    uint32_t    sh_type; 
    uint32_t    sh_flags; 
    uint32_t    sh_addr; 
    uint32_t    sh_offset; 
    uint32_t    sh_size; 
    uint32_t    sh_link; 
    uint32_t    sh_info; 
    uint32_t    sh_addralign; 
    uint32_t    sh_entsize;
} elf32_sheader; // 40 bytes (0x28)

// Load the ELF file from the SD card to memory, starting at address `base`
static void inline load_elf(char *filename, char *base) {
    int size = my_fat32_read(filename, base);
    if (size < 0)
        panic("[MY-ELF] Couldn't read ELF file from the FAT32 filesystem\n");
    else
        printk("[MY-ELF] ELF file loaded into memory (%x - %x)\n", base, base + size);
}

// Verify the ELF header. We'll do only a few checks as an exercise.
// Refer to 1-3 of ELF.pdf for the ELF header format
static void inline verify_elf(elf32_header *e_header) {
    // 1. Verify the ELF file magic number, which is 0x7f, 'E', 'L', 'F', in that order
    // Refer to 1-3 and 1-5.
    // todo("Verify the ELF file magic number");
    if (e_header->e_ident[0] != 0x7f || e_header->e_ident[1] != 'E' || 
        e_header->e_ident[2] != 'L' || e_header->e_ident[3] != 'F')
        panic("[MY-ELF] Not an ELF file!\n");
    else
        printk("[MY-ELF] ELF file magic number verified\n");

    // 2. Verify that the ELF file is either executable or shared object
    // Refer to 1-3.
    // todo("Verify that the ELF file is either executable or shared object");
    if (e_header->e_type != 2 && e_header->e_type != 3)
        panic("[MY-ELF] Not an executable or shared object ELF file!\n");
    else
        printk("[MY-ELF] ELF file type verified\n");

    // 3. Verify that the ELF file is for 32-bit architecture
    // Refer to 1-5 and 1-6.
    // todo("Verify that the ELF file is for 32-bit architecture");
    if (e_header->e_ident[4] != 1)
        panic("[MY-ELF] Not a 32-bit ELF file!\n");
    else
        printk("[MY-ELF] ELF file architecture verified\n");
}

#endif
