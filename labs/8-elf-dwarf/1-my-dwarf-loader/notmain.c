#include "rpi.h"
#include "my-elf-loader.h"
#include "my-dwarf-loader.h"

void checkoff_loader(my_dwarf_sections *dwarf_sections) {
    assert(dwarf_sections->e_sheaders != NULL);
    assert(dwarf_sections->debug_line != NULL);
    assert(dwarf_sections->debug_line_end != NULL);

    elf32_header *e_header = dwarf_sections->e_header;
    elf32_sheader *e_sheaders = dwarf_sections->e_sheaders;
    char *shstrtab = (char *)((uint32_t)e_header + e_sheaders[e_header->e_shstrndx].sh_offset);

    // Super naive checkoff: just check the first byte of each section
    for (int i = 0; i < e_header->e_shnum; i++) {
        char *section_name = shstrtab + e_sheaders[i].sh_name;
        if (strcmp(section_name, ".debug_line") == 0)
            assert(*(uint8_t *)dwarf_sections->debug_line == *(uint8_t *)((uint32_t)e_header + e_sheaders[i].sh_offset));
    }

    printk("DWARF LOADER TEST PASSED\n");
}

void notmain() {
    // Must load the ELF executable to 0x0, as it is position-dependent
    char *exec_base = (char *)0x0;
    char *exec_filename = "TEST-2.ELF";

    // Load the ELF file from the SD card to memory
    // With this, our ELF file is fully loaded in memory, starting at 0x0
    load_elf(exec_filename, exec_base);

    // Verify the ELF header
    elf32_header *e_header = (elf32_header *)exec_base;
    verify_elf(e_header);

    // Initialize the my_dwarf_sections struct
    my_dwarf_sections dwarf_sections;
    dwarf_sections_init(e_header, &dwarf_sections);

    // Checkoff: Verify that the debug sections are successfully loaded into memory
    checkoff_loader(&dwarf_sections);
}
