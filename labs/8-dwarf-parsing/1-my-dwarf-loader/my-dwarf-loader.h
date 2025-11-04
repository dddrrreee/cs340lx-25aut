#ifndef __MY_DWARF_LOADER_H__
#define __MY_DWARF_LOADER_H__

#include "rpi.h"
#include "my-elf-loader.h"

// Helper struct for the debugger.
// Just a bunch of useful addresses.
typedef struct {
    // ELF headers
    elf32_header *e_header;
    elf32_sheader *e_sheaders;

    // Debug sections
    void *debug_info;
    void *debug_info_end;
    void *debug_abbrev;
    void *debug_abbrev_end;
    void *debug_aranges;
    void *debug_aranges_end;
    void *debug_line;
    void *debug_line_end;
    void *debug_str;
    void *debug_str_end;
    void *debug_frame;
    void *debug_frame_end;
    void *debug_loc;
    void *debug_loc_end;
} my_dwarf_sections;

// Initialize the my_dwarf_sections struct
// This function loads the debug sections into the my_dwarf_sections struct
// Specifically, the sections are loaded into the heap, and the pointers are stored in the my_dwarf_sections struct
// This is required because the debug sections are not assigned an address like other ELF sections after linking, 
// and thus we cannot naively load them into memory along with other sections (e.g., the .bss section will overwrite
// whatever is after itself)
static void inline dwarf_sections_init(elf32_header *e_header, my_dwarf_sections *dwarf_sections) {
    // Get the section header
    elf32_sheader *e_sheaders = (elf32_sheader *)((uint32_t)e_header + e_header->e_shoff);

    // Load into the my_dwarf_sections struct
    dwarf_sections->e_header = e_header;
    dwarf_sections->e_sheaders = e_sheaders;

    // Get the section name string table
    char *shstrtab = (char *)((uint32_t)e_header + e_sheaders[e_header->e_shstrndx].sh_offset);

    // Initialize the elf32_dwarf_sections struct
    dwarf_sections->debug_info = NULL;
    dwarf_sections->debug_info_end = NULL;
    dwarf_sections->debug_abbrev = NULL;
    dwarf_sections->debug_abbrev_end = NULL;
    dwarf_sections->debug_aranges = NULL;
    dwarf_sections->debug_aranges_end = NULL;
    dwarf_sections->debug_line = NULL;
    dwarf_sections->debug_line_end = NULL;
    dwarf_sections->debug_str = NULL;
    dwarf_sections->debug_str_end = NULL;
    dwarf_sections->debug_frame = NULL;
    dwarf_sections->debug_frame_end = NULL;
    dwarf_sections->debug_loc = NULL;
    dwarf_sections->debug_loc_end = NULL;

    // Iterate through the section header table and search for debugger-related sections
    for (int i = 0; i < e_header->e_shnum; i++) {
        char *section_name = shstrtab + e_sheaders[i].sh_name;
        if (strcmp(section_name, ".debug_line") == 0) {
            // Step 1. Allocate heap memory for the debug line section, using the section size information from the section header
            todo("Allocate heap memory for the debug line section");
            // dwarf_sections->debug_line = kmalloc(??);

            // Step 2. Set the end address of the debug line section
            todo("Set the end address of the debug line section");
            // dwarf_sections->debug_line_end = (void *)((uint32_t)dwarf_sections->debug_line + ??);

            // Step 3. Copy the debug line section from the ELF file to the heap
            todo("Copy the debug line section from the ELF file to the heap");
            // memcpy(dwarf_sections->debug_line, (void *)((uint32_t)e_header + ??), ??);
        }
        // You can load other debug sections here for extension!
    }

    printk("[MY-DWARF] Debug sections loaded into memory\n");
}

#endif // __MY_DWARF_LOADER_H__
