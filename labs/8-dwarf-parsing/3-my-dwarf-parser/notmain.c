#include "rpi.h"
#include "../1-my-dwarf-loader/my-elf-loader.h"
#include "../1-my-dwarf-loader/my-dwarf-loader.h"
#include "my-dwarf-parser.h"

static void inline checkoff_parser(my_dwarf_line_entry *addr_table) {
    printk("----------------------------------------------------------------\n");
    for (int i = 0; i < DWARF_LINE_TABLE_SIZE; i++) {
        if (addr_table[i].is_valid)
            printk("%x: %s:%u:%u\n", 
                addr_table[i].address, addr_table[i].file_name, 
                addr_table[i].line_number, addr_table[i].column_number);
    }
    printk("----------------------------------------------------------------\n");
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

    // Parse DWARF .debug_line section
    my_dwarf_line_entry *addr_table = kmalloc(
        sizeof(my_dwarf_line_entry) * DWARF_LINE_TABLE_SIZE);
    memset(addr_table, 0, sizeof(my_dwarf_line_entry) * DWARF_LINE_TABLE_SIZE);
    parse_debug_line(addr_table, &dwarf_sections);

    // Checkoff: Print the addr_table
    checkoff_parser(addr_table);
}
