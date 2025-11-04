#include "rpi.h"
#include "my-dwarf-utils.h"

static void inline checkoff_uleb128() {
    uint64_t decoded[4] = {
        0b0, 
        0b100, 
        0b1011100010011111,
        0b111101001111000011001011001010010010101101011010011100
    };
    uint8_t encoded[] = {
        0b0,
        0b100,
        0b10011111, 0b11110001, 0b10,
        0b10011100, 0b10101101, 0b10101011, 0b11010010, 0b10101100, 0b10000110, 0b11001111, 0b11110
    };

    uint8_t *ptr = &encoded[0];
    for (int i = 0; i < 4; i++) {
        uint64_t result = read_uleb128(&ptr);
        assert(result == decoded[i]);
    }
    assert(ptr == &encoded[sizeof(encoded)]);

    printk("ULEB128 TEST PASSED\n");
}

static void inline checkoff_sleb128() {
    int64_t decoded[6] = {
        0, // 0b0
        4, // 0b100
        -5, // 0b11111011
        47263, // 0b1011100010011111
        -8462, // 0b101111011110010
        -17236162419283612LL // 0b1000010110000111100110100110101101101010010100101100100
    };

    uint8_t encoded[] = {
        0b0,
        0b100,
        0b01111011,
        0b10011111, 0b11110001, 0b10,
        0b11110010, 0b10111101, 0b01111111,
        0b11100100, 0b11010010, 0b11010100, 0b10101101, 0b11010011, 0b11111001, 0b10110000, 0b01100001
    };

    uint8_t *ptr = &encoded[0];
    for (int i = 0; i < 6; i++) {
        int64_t result = read_sleb128(&ptr);
        assert(result == decoded[i]);
    }
    assert(ptr == &encoded[sizeof(encoded)]);

    printk("SLEB128 TEST PASSED\n");
}

static void inline checkoff_line_program_header() {
    uint8_t encoded[] = {
        0x76, 0x01, 0x00, 0x00, // unit_length = 374
        0x03, 0x00, // version = 2
        0x39, 0x00, 0x00, 0x00, // header_length = 57
        0x01, // min_inst_length = 1
        0x01, // default_is_stmt = 1
        0xFB, // line_base = -5
        0x0E, // line_range = 14
        0x0D, // opcode_base = 13
        0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, // standard_opcode_lengths
        's', 'r', 'c', 0x00, 'i', 'n', 'c', 'l', 'u', 'd', 'e', 0x00, 'l', 'i', 'b', 0x00, 0x00, // include_dirs: "src", "include", "lib", ""
        'm','a','i','n','.', 'c', 0x00, 0xC1, 0x00, 0x00, 0x7B, // file #1: "main.c", followed by random 3 uleb128
        'u','t','i','l','.', 'c', 0x00, 0x02, 0xE8, 0x07, 0xD8, 0xC0, 0x03, // file #2: "util.c", followed by random 3 uleb128
        0x00 // files terminator
    };

    my_dwarf_line_program_header line_program_header;

    uint8_t *ptr_start = &encoded[0];
    uint8_t *ptr = ptr_start;
    uint8_t *ptr_end = parse_line_program_header(&line_program_header, &ptr);

    // Check that the pointer advanced appropriately
    assert(ptr == &encoded[sizeof(encoded)]);
    assert(ptr_end == ptr_start + 4 + 374);

    // Scalar fields
    assert(line_program_header.unit_length == 374);
    assert(line_program_header.version == DEBUG_LINE_DWARF_VERSION);
    assert(line_program_header.header_length == 57);
    assert(line_program_header.min_inst_length == 1);
    assert(line_program_header.default_is_stmt == 1);
    assert(line_program_header.line_base == -5);
    assert(line_program_header.line_range == 14);
    assert(line_program_header.opcode_base == 13);

    // Standard opcode lengths
    const uint8_t standard_opcode_lengths[12] = {
        0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01};
    for (int i = 0; i < 12; i++)
        assert(line_program_header.standard_opcode_lengths[i] == standard_opcode_lengths[i]);

    // Include dirs
    assert(strcmp(line_program_header.include_dirs[0], "src") == 0);
    assert(strcmp(line_program_header.include_dirs[1], "include") == 0);
    assert(strcmp(line_program_header.include_dirs[2], "lib") == 0);

    // File table
    assert(strcmp(line_program_header.file_table[0], "main.c") == 0);
    assert(strcmp(line_program_header.file_table[1], "util.c") == 0);

    printk("LINE PROGRAM HEADER PARSER TEST PASSED\n");
}

static void inline checkoff_line_state() {
    my_dwarf_line_state line_state;
    init_line_state(&line_state, 0);

    assert(!line_state.address);
    assert(line_state.file == 1);
    assert(line_state.line == 1);
    assert(!line_state.column);
    assert(!line_state.is_stmt);
    assert(!line_state.basic_block);
    assert(!line_state.end_sequence);
    assert(!line_state.prologue_end);
    assert(!line_state.epilogue_begin);
    assert(!line_state.isa);
    
    printk("INIT LINE STATE TEST PASSED\n");
}

void notmain() {
    // No need to load the ELF file, as we are just testing the utils
    checkoff_uleb128();
    checkoff_sleb128();
    checkoff_line_program_header();
    checkoff_line_state();
}
