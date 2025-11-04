#ifndef __MY_DWARF_PARSER_H__
#define __MY_DWARF_PARSER_H__

#include "rpi.h"
#include "../2-my-dwarf-utils/my-dwarf-macros.h"
#include "../2-my-dwarf-utils/my-dwarf-utils.h"

// A struct that represents a single line entry
typedef struct {
    uint8_t is_valid;
    uint32_t address;
    char file_name[32];
    uint32_t line_number;
    uint32_t column_number;
} my_dwarf_line_entry;

// A helper function to add an entry to the address table.
// You do NOT have to use this. It's here for your convenience :)
static void inline insert_entry(
    my_dwarf_line_entry *addr_table,
    uint32_t address,
    const char *file_name,
    uint32_t line_number,
    uint32_t column_number
) {
    assert(strlen(file_name) < 32);

    // Create the entry
    my_dwarf_line_entry new_entry;
    new_entry.address = address;
    strcpy(new_entry.file_name, file_name);
    new_entry.line_number = line_number;
    new_entry.column_number = column_number;
    new_entry.is_valid = 1;

    // Insert into addr_table
    // These are arbitrary rules irrelevant to DWARF specs,
    // but convenient for creating the addr2line tool
    assert(address < DWARF_LINE_TABLE_SIZE);
    my_dwarf_line_entry existing_entry = addr_table[address];
    if (!existing_entry.is_valid) {
        // Case 1. No entry at this address. Simply insert the new entry.
        addr_table[address] = new_entry;
    } else if (strcmp(existing_entry.file_name, new_entry.file_name) != 0) {
        // Case 2. The entry at this address has a different file name.
        // If the new file name ends with ".c", insert the new entry.
        int len = strlen(existing_entry.file_name);
        if (new_entry.file_name[len - 1] == 'c' && new_entry.file_name[len - 2] == '.')
            addr_table[address] = new_entry;
    } else if (existing_entry.line_number > new_entry.line_number) {
        // Case 3. The entry at this address has a different line number.
        // If the new line number is smaller, insert the new entry.
        addr_table[address] = new_entry;
    } else if (existing_entry.column_number > new_entry.column_number) {
        // Case 4. The entry at this address has a different column number.
        // If the new column number is smaller, insert the new entry.
        addr_table[address] = new_entry;
    }
}

// Note that .debug_line section version is DWARF 3, not 4
// DWARF is meant to be producer-consumer-independent, thus each section has its own version
// .debug_line section is a series of header + program
// The program is a series of opcodes that drive a state machine to reconstruct the line number matrix
// DWARF 3 spec pg 92 - 104
static void inline parse_debug_line(my_dwarf_line_entry *addr_table, my_dwarf_sections *dwarf_sections) {
    printk("[MY-DWARF] Parsing .debug_line...\n");

    uint8_t *debug_line_start = dwarf_sections->debug_line;
    uint8_t *debug_line_end = dwarf_sections->debug_line_end;

    uint8_t *ptr = debug_line_start;

    while (ptr < debug_line_end) {        
        my_dwarf_line_program_header line_program_header;
        uint8_t *current_cu_end = parse_line_program_header(&line_program_header, &ptr);

        // Initialize the line number state machine
        my_dwarf_line_state line_state;
        init_line_state(&line_state, line_program_header.default_is_stmt);

        // Parse the line number program
        // The ultimate goal is to reconstruct the line number matrix
        // which will let us build a mapping <address> -> <file, line, column, ...>
        while (ptr < current_cu_end) {
            uint8_t opcode = *ptr++;

            if (opcode >= line_program_header.opcode_base) {
                // Case 1. Special opcode (no operands)
                // DWARF 3 spec pg 98-100 (6.2.5.1 Special Opcodes)
                // Note: You can use the provided udiv and umod functions in my-dwarf-utils.h

                // First, caculate adjusted opcode, addr_inc and line_inc
                todo("Calculate adjusted opcode, addr_inc and line_inc");
                // uint8_t adjusted_opcode = ??;
                // uint32_t addr_inc = ??;
                // int32_t line_inc = ??;

                // Second, apply all the changes required by special opcodes (page 98)
                todo("Apply all the changes required by special opcodes");
                // line_state.address += ??;
                // line_state.line += ??;
                // line_state.basic_block = ??;
                // line_state.prologue_end = ??;
                // line_state.epilogue_begin = ??;

                // Finally, page 98 states that we should append a row to the line number matrix. So let's do that.
                todo("Append a row to the line number matrix");
                // insert_entry(?, ?, line_program_header.file_table[line_state.file - 1], ?, ?);
            } else if (opcode > 0) {
                // Case 2. Standard opcode (zero or more LEB128 operands, except for DW_LNS_fixed_advance_pc)
                // DWARF 3 spec pg 100-103 (6.2.5.2 Standard Opcodes)

                switch (opcode) {
                    case DW_LNS_copy: {
                        // Append row to the line number matrix and reset basic_block, prologue_end, and epilogue_begin
                        todo("Handle DW_LNS_copy");
                        // insert_entry(?, ?, line_program_header.file_table[line_state.file - 1], ?, ?);
                        // line_state.basic_block = ??;
                        // line_state.prologue_end = ??;
                        // line_state.epilogue_begin = ??;
                        break;
                    }

                    case DW_LNS_advance_pc: {
                        // Advance the current address of the state machine.
                        // Read an ULEB128 value and multiply it by min_inst_length to determine the amount to increase
                        todo("Handle DW_LNS_advance_pc");
                        // uint64_t addr_inc = ??;
                        // line_state.address += ??;
                        break;
                    }

                    case DW_LNS_advance_line: {
                        // Advance the current line number of the state machine by the given SLEB128 value
                        todo("Handle DW_LNS_advance_line");
                        // int64_t line_inc = ??;
                        // line_state.line += ??;
                        break;
                    }

                    case DW_LNS_set_file: {
                        // Set the file index of the state machine, given as an ULEB128 value
                        todo("Handle DW_LNS_set_file");
                        // line_state.file = ??;
                        break;
                    }

                    case DW_LNS_set_column: {
                        // Set the column number of the state machine, given as an ULEB128 value
                        todo("Handle DW_LNS_set_column");
                        // line_state.column = ??;
                        break;
                    }

                    case DW_LNS_negate_stmt: {
                        // Logically negate the is_stmt value
                        todo("Handle DW_LNS_negate_stmt");
                        // line_state.is_stmt = ??;
                        break;
                    }

                    case DW_LNS_set_basic_block: {
                        // Set the basic_block field
                        todo("Handle DW_LNS_set_basic_block");
                        // line_state.basic_block = ??;
                        break;
                    }

                    case DW_LNS_const_add_pc: {
                        // Process as if this is special op (case 1) with opcode 255
                        todo("Handle DW_LNS_const_add_pc");
                        // uint8_t addr_inc = ??;
                        // line_state.address += ??;
                        break;
                    }

                    case DW_LNS_fixed_advance_pc: {
                        // Read a uint16 value and add it to the address field of line state machine
                        // Note that we do not multiply by min_inst_length here
                        todo("Handle DW_LNS_fixed_advance_pc");
                        // uint16_t addr_inc = ??;
                        // line_state.address += ??;
                        break;
                    }

                    case DW_LNS_set_prologue_end: {
                        // Set the prologue_end field
                        todo("Handle DW_LNS_set_prologue_end");
                        // line_state.prologue_end = ??;
                        break;
                    }

                    case DW_LNS_set_epilogue_begin: {
                        // Set the epilogue_begin field
                        todo("Handle DW_LNS_set_epilogue_begin");
                        // line_state.epilogue_begin = ??;
                        break;
                    }

                    case DW_LNS_set_isa: {
                        // Read a single ULEB128 value and set it to the isa field
                        todo("Handle DW_LNS_set_isa");
                        // line_state.isa = ??;
                        break;
                    }

                    default: {
                        panic("Unknown opcode: %u", opcode);
                        break;
                    }
                }

            } else if (opcode == 0) {
                // Case 3. Extended opcode (first byte zero, LEB128 length, then the instruction itself)
                // DWARF 3 spec pg 103-104 (6.2.5.3 Extended Opcodes)
                
                uint64_t length = read_uleb128(&ptr);
                uint8_t extended_opcode = *ptr++;

                switch (extended_opcode) {
                    case DW_LNE_end_sequence: {
                        // Set the end_sequence field and reset the line number state machine
                        // Note: The spec tells us to append a row to the line number matrix
                        //       But this is redundant. You don't need to do it.
                        todo("Handle DW_LNS_end_sequence");
                        // line_state.end_sequence = ??;
                        // init_line_state(?, ?);
                        break;
                    }

                    case DW_LNE_set_address: {
                        // Read a single uint32 (since our arch is 32b) and set it as the address
                        // Note: The spec says this is a relocatable address, but our executables are all 
                        //       statically linked, so this is the final runtime address
                        todo("Handle DW_LNS_set_address");
                        // line_state.address = ??;
                        break;
                    }

                    case DW_LNE_define_file: {
                        panic("DW_LNE_define_file should not occur for our case.");
                        break;
                    }

                    case DW_LNE_set_discriminator: {
                        // Amazingly, DW_LNE_set_discriminator exists in our executables even though this op 
                        // is part of DWARF 4 and our executable uses DWARF 3. It turns out that compiler 
                        // toolchains do not strictly abide by the DWARF specifications. 
                        // We'll just read & skip it.
                        read_uleb128(&ptr);
                        break;
                    }

                    default: {
                        panic("Unknown extended opcode: %u", extended_opcode);
                        break;
                    }
                }

            } else {
                panic("Should not reach here.");
            }
        }

        // Move to the next compilation unit
        ptr = current_cu_end;
    }
}

#endif // __MY_DWARF_PARSER_H__
