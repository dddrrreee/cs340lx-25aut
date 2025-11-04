#ifndef __MY_DWARF_UTILS_H__
#define __MY_DWARF_UTILS_H__

#include "rpi.h"
#include "my-dwarf-macros.h"

// Line number program header
// DWARF 3 spec page 95-98
typedef struct {
    uint32_t unit_length;
    uint16_t version;
    uint32_t header_length;
    uint8_t min_inst_length;
    uint8_t default_is_stmt;
    int8_t line_base;
    uint8_t line_range;
    uint8_t opcode_base;
    uint8_t standard_opcode_lengths[32];
    const char *include_dirs[32];
    const char *file_table[32];
} my_dwarf_line_program_header;

// Line state machine, used to reconstruct the line number matrix
// The field sizes don't matter, we use them to progress the hypothetical state machine
// DWARF 3 spec page 93-94
typedef struct {
    uint32_t address;
    uint32_t file;
    uint32_t line;
    uint32_t column;
    uint8_t is_stmt;
    uint8_t basic_block;
    uint8_t end_sequence;
    uint8_t prologue_end;
    uint8_t epilogue_begin;
    uint32_t isa;
} my_dwarf_line_state;

// To avoid misalignment issues
static inline uint16_t read_u16(const uint8_t *p) {
    return p[0] | ((uint16_t)p[1] << 8);
}

// To avoid misalignment issues
static inline uint32_t read_u32(const uint8_t *p) {
    return p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

// Read the DWARF ULEB128 encoding and progress the pointer
// This is variable-length unsigned integer encoding
// The MSB in each byte is used to indicate if there are more bytes to read
// Thus 7 bits in each byte are used to store the actual data
// DWARF 3 spec page 139-141
uint64_t read_uleb128(uint8_t **data) {
    uint64_t result = 0;
    int shift = 0;
    uint8_t *ptr = *data;
    while (1) {
        // Parse each byte in the ULEB128 encoding
        todo("Parse each byte in the ULEB128 encoding");
        // uint8_t byte = *ptr++;
        // result = ?
        // if ((byte & 0x80) == 0) break;
        // shift += ?;
    }
    *data = ptr;
    return result;
}

// Similar to read_uleb128, but reads a signed LEB128 encoding
// DWARF 3 spec page 139-141
int64_t read_sleb128(uint8_t **data) {
    int64_t result = 0;
    int shift = 0;
    uint8_t *ptr = *data;
    
    uint8_t byte;
    while (1) {
        // Parse each byte in the SLEB128 encoding
        todo("Parse each byte in the SLEB128 encoding");
        // byte = *ptr++;
        // result = ?;
        // shift += ?;
        // if ((byte & 0x80) == 0) break;
    }

    // Sign-extend if negative
    todo("Sign-extend if negative");
    // if ((shift < 64) && (byte & 0x40)) {
    //     result = ??
    // }

    *data = ptr;
    return result;
}

// Parse the line program header
// DWARF 3 spec page 95-98
static inline uint8_t *parse_line_program_header(my_dwarf_line_program_header *line_program_header, uint8_t **line_ptr) {
    // 1. unit_length
    // length for this compilation unit excluding the unit_length field itself
    line_program_header->unit_length = read_u32(*line_ptr); 
    *line_ptr += 4; // read_u32 does not advance the pointer

    // for advancing to the next unit (must exclude the unit_length field itself)
    uint8_t *end_ptr = *line_ptr + line_program_header->unit_length; 

    // 2. version. 
    // Should be 3 for our case
    todo("Parse the version");
    // line_program_header->version = ??; // remember to advance the pointer!
    assert(line_program_header->version == DEBUG_LINE_DWARF_VERSION);

    // 3. header_length
    todo("Parse the header length");
    // line_program_header->header_length = ??;

    // 4. minimum_instruction_length
    todo("Parse the minimum instruction length");
    // line_program_header->min_inst_length = ??;

    // 5. default_is_stmt
    todo("Parse the default is statement");
    // line_program_header->default_is_stmt = ??;

    // 6. line_base
    todo("Parse the line base");
    // line_program_header->line_base = ??;

    // 7. line_range
    todo("Parse the line range");
    // line_program_header->line_range = ??;
    assert(line_program_header->line_range > 0);

    // 8. opcode_base
    todo("Parse the opcode base");
    // line_program_header->opcode_base = ??;
    assert(10 <= line_program_header->opcode_base && line_program_header->opcode_base <= 32);

    // 9. standard_opcode_lengths
    for (int i = 1; i < line_program_header->opcode_base; i++)
        line_program_header->standard_opcode_lengths[i - 1] = *(*line_ptr)++;

    // 10. include_directories
    // Instead of copying the entire string, we just save the pointer to the string
    for (int i = 0; ; i++) {
        if (i >= 32)
            panic("Include directory table overflow");
        if (strlen((char *)*line_ptr) == 0)
            break;
        line_program_header->include_dirs[i] = (char *)*line_ptr;
        *line_ptr += strlen(line_program_header->include_dirs[i]) + 1; // +1 for the null terminator
    }
    *line_ptr += 1; // for the last null terminator

    // 11. file_names
    // Instead of copying the entire string, we just save the pointer to the string
    for (int i = 0; ; i++) {
        if (i >= 32) 
            panic("File table overflow");
        if (strlen((char *)*line_ptr) == 0) 
            break;
        todo("Save the file name string pointer");
        // line_program_header->file_table[i] = ??;

        // file name index in the include directories
        // We won't save them, but we must read them to advance the pointer
        uint64_t file_name_index = read_uleb128(line_ptr);
        uint64_t last_modified_time = read_uleb128(line_ptr);
        uint64_t file_size = read_uleb128(line_ptr);
    }
    *line_ptr += 1; // for the last null terminator

    // Return the pointer to the end of the compilation unit
    return end_ptr;
}

// Initialize the line state machine
// DWARF 3 spec page 94
static inline void init_line_state(my_dwarf_line_state *line_state, uint8_t default_is_stmt) {
    // Initialize the line state machine
    todo("Initialize the line state machine");
    // line_state->address = ??
    // line_state->file = ??
    // line_state->line = ??
    // line_state->column = ??
    // line_state->is_stmt = ??
    // line_state->basic_block = ??
    // line_state->end_sequence = ??
    // line_state->prologue_end = ??
    // line_state->epilogue_begin = ??
    // line_state->isa = ??
}

static inline uint32_t udiv(uint32_t dividend, uint32_t divisor) {
    if (divisor == 0) panic("Division by zero");
    uint32_t quotient = 0;
    while (dividend >= divisor) {
        dividend -= divisor;
        quotient++;
    }
    return quotient;
}

static inline uint32_t umod(uint32_t dividend, uint32_t divisor) {
    if (divisor == 0) panic("Division by zero");
    while (dividend >= divisor)
        dividend -= divisor;
    return dividend;
}

#endif // __MY_DWARF_UTILS_H__
