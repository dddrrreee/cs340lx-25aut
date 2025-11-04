#ifndef __MY_DWARF_MACROS_H__
#define __MY_DWARF_MACROS_H__

#include "rpi.h"

// Our compiler toolchain uses DWARF 4 for debug_info, DWARF 3 for debug_line, and DWARF 1 for debug_frame
#define DEBUG_INFO_DWARF_VERSION 4
#define DEBUG_LINE_DWARF_VERSION 3
#define DEBUG_FRAME_DWARF_VERSION 1

// Abbrev table
#define MAX_ABBREV_ATTRS 32
#define MAX_ABBREV_ENTRIES 128

// Line table
#define DWARF_LINE_TABLE_SIZE 65536

// DWARF tags (Figure 18)
#define DW_TAG_array_type 0x01
#define DW_TAG_class_type 0x02
#define DW_TAG_entry_point 0x03
#define DW_TAG_enumeration_type 0x04
#define DW_TAG_formal_parameter 0x05
#define DW_TAG_imported_declaration 0x08
#define DW_TAG_label 0x0a
#define DW_TAG_lexical_block 0x0b
#define DW_TAG_member 0x0d
#define DW_TAG_pointer_type 0x0f
#define DW_TAG_reference_type 0x10
#define DW_TAG_compile_unit 0x11
#define DW_TAG_string_type 0x12
#define DW_TAG_structure_type 0x13
#define DW_TAG_subroutine_type 0x15
#define DW_TAG_typedef 0x16 
#define DW_TAG_union_type 0x17
#define DW_TAG_unspecified_parameters 0x18
#define DW_TAG_variant 0x19
#define DW_TAG_common_block 0x1a
#define DW_TAG_common_inclusion 0x1b
#define DW_TAG_inheritance 0x1c
#define DW_TAG_inlined_subroutine 0x1d
#define DW_TAG_module 0x1e
#define DW_TAG_ptr_to_member_type 0x1f
#define DW_TAG_set_type 0x20
#define DW_TAG_subrange_type 0x21
#define DW_TAG_with_stmt 0x22
#define DW_TAG_access_declaration 0x23
#define DW_TAG_base_type 0x24
#define DW_TAG_catch_block 0x25
#define DW_TAG_const_type 0x26
#define DW_TAG_constant 0x27
#define DW_TAG_enumerator 0x28
#define DW_TAG_file_type 0x29
#define DW_TAG_friend 0x2a 
#define DW_TAG_namelist 0x2b
#define DW_TAG_namelist_item 0x2c
#define DW_TAG_packed_type 0x2d
#define DW_TAG_subprogram 0x2e
#define DW_TAG_template_type_parameter 0x2f
#define DW_TAG_template_value_parameter 0x30
#define DW_TAG_thrown_type 0x31
#define DW_TAG_try_block 0x32
#define DW_TAG_variant_part 0x33
#define DW_TAG_variable 0x34
#define DW_TAG_volatile_type 0x35
#define DW_TAG_dwarf_procedure 0x36
#define DW_TAG_restrict_type 0x37
#define DW_TAG_interface_type 0x38
#define DW_TAG_namespace 0x39
#define DW_TAG_imported_module 0x3a
#define DW_TAG_unspecified_type 0x3b
#define DW_TAG_partial_unit 0x3c
#define DW_TAG_imported_unit 0x3d
#define DW_TAG_condition 0x3f 
#define DW_TAG_shared_type 0x40
#define DW_TAG_type_unit 0x41
#define DW_TAG_rvalue_reference_type 0x42
#define DW_TAG_template_alias 0x43
#define DW_TAG_lo_user 0x4080
#define DW_TAG_hi_user 0xffff 

// Dwarf attributes (Figure 20)
#define DW_AT_sibling 0x01
#define DW_AT_location 0x02
#define DW_AT_name 0x03
#define DW_AT_ordering 0x09
#define DW_AT_byte_size 0x0b
#define DW_AT_bit_offset 0x0c
#define DW_AT_bit_size 0x0d
#define DW_AT_stmt_list 0x10
#define DW_AT_low_pc 0x11
#define DW_AT_high_pc 0x12
#define DW_AT_language 0x13
#define DW_AT_discr 0x15
#define DW_AT_discr_value 0x16
#define DW_AT_visibility 0x17
#define DW_AT_import 0x18
#define DW_AT_string_length 0x19
#define DW_AT_common_reference 0x1a
#define DW_AT_comp_dir 0x1b
#define DW_AT_const_value 0x1c
#define DW_AT_containing_type 0x1d
#define DW_AT_default_value 0x1e
#define DW_AT_inline 0x20
#define DW_AT_is_optional 0x21
#define DW_AT_lower_bound 0x22
#define DW_AT_producer 0x25
#define DW_AT_prototyped 0x27
#define DW_AT_return_addr 0x2a
#define DW_AT_start_scope 0x2c
#define DW_AT_bit_stride 0x2e
#define DW_AT_upper_bound 0x2f
#define DW_AT_abstract_origin 0x31
#define DW_AT_accessibility 0x32
#define DW_AT_address_class 0x33
#define DW_AT_artificial 0x34
#define DW_AT_base_types 0x35
#define DW_AT_calling_convention 0x36
#define DW_AT_count 0x37
#define DW_AT_data_member_location 0x38
#define DW_AT_decl_column 0x39
#define DW_AT_decl_file 0x3a
#define DW_AT_decl_line 0x3b
#define DW_AT_declaration 0x3c
#define DW_AT_discr_list 0x3d
#define DW_AT_encoding 0x3e
#define DW_AT_external 0x3f
#define DW_AT_frame_base 0x40
#define DW_AT_friend 0x41
#define DW_AT_identifier_case 0x42
#define DW_AT_macro_info 0x43
#define DW_AT_namelist_item 0x44
#define DW_AT_priority 0x45
#define DW_AT_segment 0x46
#define DW_AT_specification 0x47
#define DW_AT_static_link 0x48
#define DW_AT_type 0x49
#define DW_AT_use_location 0x4a
#define DW_AT_variable_parameter 0x4b
#define DW_AT_virtuality 0x4c
#define DW_AT_vtable_elem_location 0x4d
#define DW_AT_allocated 0x4e
#define DW_AT_associated 0x4f
#define DW_AT_data_location 0x50
#define DW_AT_byte_stride 0x51
#define DW_AT_entry_pc 0x52
#define DW_AT_use_UTF8 0x53
#define DW_AT_extension 0x54
#define DW_AT_ranges 0x55
#define DW_AT_trampoline 0x56
#define DW_AT_call_column 0x57
#define DW_AT_call_file 0x58
#define DW_AT_call_line 0x59
#define DW_AT_description 0x5a
#define DW_AT_binary_scale 0x5b
#define DW_AT_decimal_scale 0x5c
#define DW_AT_small 0x5d
#define DW_AT_decimal_sign 0x5e
#define DW_AT_digit_count 0x5f
#define DW_AT_picture_string 0x60
#define DW_AT_mutable 0x61
#define DW_AT_threads_scaled 0x62
#define DW_AT_explicit 0x63
#define DW_AT_object_pointer 0x64
#define DW_AT_endianity 0x65
#define DW_AT_elemental 0x66
#define DW_AT_pure 0x67
#define DW_AT_recursive 0x68
#define DW_AT_signature 0x69
#define DW_AT_main_subprogram 0x6a
#define DW_AT_data_bit_offset 0x6b
#define DW_AT_const_expr 0x6c
#define DW_AT_enum_class 0x6d
#define DW_AT_linkage_name 0x6e
#define DW_AT_lo_user 0x2000
#define DW_AT_hi_user 0x3fff

// Dwarf forms (Figure 21)
// Comments describe the class of the form
#define DW_FORM_addr 0x01 // address
#define DW_FORM_block2 0x03 // block
#define DW_FORM_block4 0x04 // block
#define DW_FORM_data2 0x05 // constant
#define DW_FORM_data4 0x06 // constant
#define DW_FORM_data8 0x07 // constant
#define DW_FORM_string 0x08 // string
#define DW_FORM_block 0x09 // block
#define DW_FORM_block1 0x0a // block
#define DW_FORM_data1 0x0b // constant
#define DW_FORM_flag 0x0c // flag
#define DW_FORM_sdata 0x0d // constant
#define DW_FORM_strp 0x0e // string
#define DW_FORM_udata 0x0f // constant
#define DW_FORM_ref_addr 0x10 // reference
#define DW_FORM_ref1 0x11 // reference
#define DW_FORM_ref2 0x12 // reference
#define DW_FORM_ref4 0x13 // reference
#define DW_FORM_ref8 0x14 // reference
#define DW_FORM_ref_udata 0x15 // reference
#define DW_FORM_indirect 0x16
#define DW_FORM_sec_offset 0x17
#define DW_FORM_exprloc 0x18
#define DW_FORM_flag_present 0x19 // flag
#define DW_FORM_ref_sig8 0x20 // reference

// DWARF expression opcodes (ref page 163-167)
#define DW_OP_addr 0x03 // 1 constant address
#define DW_OP_deref 0x06 // 0
#define DW_OP_const1u 0x08 // 1 1-byte constant
#define DW_OP_const1s 0x09 // 1 1-byte constant
#define DW_OP_const2u 0x0a // 1 2-byte constant
#define DW_OP_const2s 0x0b // 1 2-byte constant
#define DW_OP_const4u 0x0c // 1 4-byte constant
#define DW_OP_const4s 0x0d // 1 4-byte constant
#define DW_OP_const8u 0x0e // 1 8-byte constant
#define DW_OP_const8s 0x0f // 1 8-byte constant
#define DW_OP_constu 0x10 // 1 ULEB128 constant
#define DW_OP_consts 0x11 // 1 SLEB128 constant
#define DW_OP_dup 0x12 // 0
#define DW_OP_drop 0x13 // 0
#define DW_OP_over 0x14 // 0
#define DW_OP_pick 0x15 // 1 1-byte stack index
#define DW_OP_swap 0x16 // 0
#define DW_OP_rot 0x17 // 0
#define DW_OP_xderef 0x18 // 0
#define DW_OP_abs 0x19 // 0
#define DW_OP_and 0x1a // 0
#define DW_OP_div 0x1b // 0
#define DW_OP_minus 0x1c // 0
#define DW_OP_mod 0x1d // 0
#define DW_OP_mul 0x1e // 0
#define DW_OP_neg 0x1f // 0
#define DW_OP_not 0x20 // 0
#define DW_OP_or 0x21 // 0
#define DW_OP_plus 0x22 // 0
#define DW_OP_plus_uconst 0x23 // 1 ULEB128 addend
#define DW_OP_shl 0x24 // 0
#define DW_OP_shr 0x25 // 0
#define DW_OP_shra 0x26 // 0
#define DW_OP_xor 0x27 // 0
#define DW_OP_skip 0x2f // 1 signed 2-byte constant
#define DW_OP_bra 0x28 // 1 signed 2-byte constant
#define DW_OP_eq 0x29 // 0
#define DW_OP_ge 0x2a // 0
#define DW_OP_gt 0x2b // 0
#define DW_OP_le 0x2c // 0
#define DW_OP_lt 0x2d // 0
#define DW_OP_ne 0x2e // 0
#define DW_OP_lit0 0x30 // 0
#define DW_OP_lit1 0x31 // 0
#define DW_OP_lit2 0x32 // 0
#define DW_OP_lit3 0x33 // 0
#define DW_OP_lit4 0x34 // 0
#define DW_OP_lit5 0x35 // 0
#define DW_OP_lit6 0x36 // 0
#define DW_OP_lit7 0x37 // 0
#define DW_OP_lit8 0x38 // 0
#define DW_OP_lit9 0x39 // 0
#define DW_OP_lit10 0x3a // 0
#define DW_OP_lit11 0x3b // 0
#define DW_OP_lit12 0x3c // 0
#define DW_OP_lit13 0x3d // 0
#define DW_OP_lit14 0x3e // 0
#define DW_OP_lit15 0x3f // 0
#define DW_OP_lit16 0x40 // 0
#define DW_OP_lit17 0x41 // 0
#define DW_OP_lit18 0x42 // 0
#define DW_OP_lit19 0x43 // 0
#define DW_OP_lit20 0x44 // 0
#define DW_OP_lit21 0x45 // 0
#define DW_OP_lit22 0x46 // 0
#define DW_OP_lit23 0x47 // 0
#define DW_OP_lit24 0x48 // 0
#define DW_OP_lit25 0x49 // 0
#define DW_OP_lit26 0x4a // 0
#define DW_OP_lit27 0x4b // 0
#define DW_OP_lit28 0x4c // 0
#define DW_OP_lit29 0x4d // 0
#define DW_OP_lit30 0x4e // 0
#define DW_OP_lit31 0x4f // 0
#define DW_OP_reg0 0x50 // 0
#define DW_OP_reg1 0x51 // 0
#define DW_OP_reg2 0x52 // 0
#define DW_OP_reg3 0x53 // 0
#define DW_OP_reg4 0x54 // 0
#define DW_OP_reg5 0x55 // 0
#define DW_OP_reg6 0x56 // 0
#define DW_OP_reg7 0x57 // 0
#define DW_OP_reg8 0x58 // 0
#define DW_OP_reg9 0x59 // 0
#define DW_OP_reg10 0x5a // 0
#define DW_OP_reg11 0x5b // 0
#define DW_OP_reg12 0x5c // 0
#define DW_OP_reg13 0x5d // 0
#define DW_OP_reg14 0x5e // 0
#define DW_OP_reg15 0x5f // 0
#define DW_OP_reg16 0x60 // 0
#define DW_OP_reg17 0x61 // 0
#define DW_OP_reg18 0x62 // 0
#define DW_OP_reg19 0x63 // 0
#define DW_OP_reg20 0x64 // 0
#define DW_OP_reg21 0x65 // 0
#define DW_OP_reg22 0x66 // 0
#define DW_OP_reg23 0x67 // 0
#define DW_OP_reg24 0x68 // 0
#define DW_OP_reg25 0x69 // 0
#define DW_OP_reg26 0x6a // 0
#define DW_OP_reg27 0x6b // 0
#define DW_OP_reg28 0x6c // 0
#define DW_OP_reg29 0x6d // 0
#define DW_OP_reg30 0x6e // 0
#define DW_OP_reg31 0x6f // 0
#define DW_OP_breg0 0x70 // 1
#define DW_OP_breg1 0x71 // 1
#define DW_OP_breg2 0x72 // 1
#define DW_OP_breg3 0x73 // 1
#define DW_OP_breg4 0x74 // 1
#define DW_OP_breg5 0x75 // 1
#define DW_OP_breg6 0x76 // 1
#define DW_OP_breg7 0x77 // 1
#define DW_OP_breg8 0x78 // 1
#define DW_OP_breg9 0x79 // 1
#define DW_OP_breg10 0x7a // 1
#define DW_OP_breg11 0x7b // 1
#define DW_OP_breg12 0x7c // 1
#define DW_OP_breg13 0x7d // 1
#define DW_OP_breg14 0x7e // 1
#define DW_OP_breg15 0x7f // 1
#define DW_OP_breg16 0x80 // 1
#define DW_OP_breg17 0x81 // 1
#define DW_OP_breg18 0x82 // 1
#define DW_OP_breg19 0x83 // 1
#define DW_OP_breg20 0x84 // 1
#define DW_OP_breg21 0x85 // 1
#define DW_OP_breg22 0x86 // 1
#define DW_OP_breg23 0x87 // 1
#define DW_OP_breg24 0x88 // 1
#define DW_OP_breg25 0x89 // 1
#define DW_OP_breg26 0x8a // 1
#define DW_OP_breg27 0x8b // 1
#define DW_OP_breg28 0x8c // 1
#define DW_OP_breg29 0x8d // 1
#define DW_OP_breg30 0x8e // 1
#define DW_OP_breg31 0x8f // 1
#define DW_OP_regx 0x90 // 1 ULEB128 register
#define DW_OP_fbreg 0x91 // 1 SLEB128 offset
#define DW_OP_bregx 0x92 // 2 ULEB128 register followed by
#define DW_OP_piece 0x93 // 1 ULEB128 size of piece addressed
#define DW_OP_deref_size 0x94 // 1 1-byte size of data retrieved
#define DW_OP_xderef_size 0x95 // 1 1-byte size of data retrieved
#define DW_OP_nop 0x96 // 0
#define DW_OP_push_object_address 0x97 // 0
#define DW_OP_call2 0x98 // 1 2-byte offset of DIE
#define DW_OP_call4 0x99 // 1 4-byte offset of DIE
#define DW_OP_call_ref 0x9a // 1 4- or 8-byte offset of DIE
#define DW_OP_form_tls_address 0x9b // 0
#define DW_OP_call_frame_cfa 0x9c // 0
#define DW_OP_bit_piece 0x9d // 2 ULEB128 size followed by
#define DW_OP_implicit_value 0x9e // 2 ULEB128 size followed by block
#define DW_OP_stack_value 0x9f // 0
#define DW_OP_lo_user 0xe0
#define DW_OP_hi_user 0xff

// DWARF base types (ref page 168-169)
#define DW_ATE_address 0x01
#define DW_ATE_boolean 0x02
#define DW_ATE_complex_float 0x03
#define DW_ATE_float 0x04
#define DW_ATE_signed 0x05
#define DW_ATE_signed_char 0x06
#define DW_ATE_unsigned 0x07
#define DW_ATE_unsigned_char 0x08
#define DW_ATE_imaginary_float 0x09
#define DW_ATE_packed_decimal 0x0a
#define DW_ATE_numeric_string 0x0b
#define DW_ATE_edited 0x0c
#define DW_ATE_signed_fixed 0x0d
#define DW_ATE_unsigned_fixed 0x0e
#define DW_ATE_decimal_float 0x0f
#define DW_ATE_UTF 0x10
#define DW_ATE_lo_user 0x80
#define DW_ATE_hi_user 0xff

// Dwarf 3 line number opcodes (DWARF 3 ref page 155-156)
#define DW_LNS_copy 0x01
#define DW_LNS_advance_pc 0x02
#define DW_LNS_advance_line 0x03
#define DW_LNS_set_file 0x04
#define DW_LNS_set_column 0x05
#define DW_LNS_negate_stmt 0x06
#define DW_LNS_set_basic_block 0x07
#define DW_LNS_const_add_pc 0x08
#define DW_LNS_fixed_advance_pc 0x09
#define DW_LNS_set_prologue_end 0x0a
#define DW_LNS_set_epilogue_begin 0x0b
#define DW_LNS_set_isa 0x0c

// Dwarf 3 line number extended opcodes (DWARF 3 ref page 156)
#define DW_LNE_end_sequence 0x01
#define DW_LNE_set_address 0x02
#define DW_LNE_define_file 0x03
#define DW_LNE_set_discriminator 0x04
#define DW_LNE_lo_user 0x80
#define DW_LNE_hi_user 0xff

// Dwarf 1 call frame opcodes
#define DW_CFA_nop 0x0
#define DW_CFA_set_loc 0x01
#define DW_CFA_advance_loc1 0x02
#define DW_CFA_advance_loc2 0x03
#define DW_CFA_advance_loc4 0x04
#define DW_CFA_offset_extended 0x05
#define DW_CFA_restore_extended 0x06
#define DW_CFA_undefined 0x07
#define DW_CFA_same_value 0x08
#define DW_CFA_register 0x09
#define DW_CFA_remember_state 0x0a
#define DW_CFA_restore_state 0x0b
#define DW_CFA_def_cfa 0x0c
#define DW_CFA_cfa_register 0x0d
#define DW_CFA_def_cfa_offset 0x0e
#define DW_CFA_lo_user 0x1c
#define DW_CFA_hi_user 0x3f

// Helper function to convert DWARF tags to human-readable names
static inline const char* dwarf_tag_name(uint64_t tag) {
    switch (tag) {
        case DW_TAG_array_type: return "DW_TAG_array_type";
        case DW_TAG_class_type: return "DW_TAG_class_type";
        case DW_TAG_entry_point: return "DW_TAG_entry_point";
        case DW_TAG_enumeration_type: return "DW_TAG_enumeration_type";
        case DW_TAG_formal_parameter: return "DW_TAG_formal_parameter";
        case DW_TAG_imported_declaration: return "DW_TAG_imported_declaration";
        case DW_TAG_label: return "DW_TAG_label";
        case DW_TAG_lexical_block: return "DW_TAG_lexical_block";
        case DW_TAG_member: return "DW_TAG_member";
        case DW_TAG_pointer_type: return "DW_TAG_pointer_type";
        case DW_TAG_reference_type: return "DW_TAG_reference_type";
        case DW_TAG_compile_unit: return "DW_TAG_compile_unit";
        case DW_TAG_string_type: return "DW_TAG_string_type";
        case DW_TAG_structure_type: return "DW_TAG_structure_type";
        case DW_TAG_subroutine_type: return "DW_TAG_subroutine_type";
        case DW_TAG_typedef: return "DW_TAG_typedef";
        case DW_TAG_union_type: return "DW_TAG_union_type";
        case DW_TAG_unspecified_parameters: return "DW_TAG_unspecified_parameters";
        case DW_TAG_variant: return "DW_TAG_variant";
        case DW_TAG_common_block: return "DW_TAG_common_block";
        case DW_TAG_common_inclusion: return "DW_TAG_common_inclusion";
        case DW_TAG_inheritance: return "DW_TAG_inheritance";
        case DW_TAG_inlined_subroutine: return "DW_TAG_inlined_subroutine";
        case DW_TAG_module: return "DW_TAG_module";
        case DW_TAG_ptr_to_member_type: return "DW_TAG_ptr_to_member_type";
        case DW_TAG_set_type: return "DW_TAG_set_type";
        case DW_TAG_subrange_type: return "DW_TAG_subrange_type";
        case DW_TAG_with_stmt: return "DW_TAG_with_stmt";
        case DW_TAG_access_declaration: return "DW_TAG_access_declaration";
        case DW_TAG_base_type: return "DW_TAG_base_type";
        case DW_TAG_catch_block: return "DW_TAG_catch_block";
        case DW_TAG_const_type: return "DW_TAG_const_type";
        case DW_TAG_constant: return "DW_TAG_constant";
        case DW_TAG_enumerator: return "DW_TAG_enumerator";
        case DW_TAG_file_type: return "DW_TAG_file_type";
        case DW_TAG_friend: return "DW_TAG_friend";
        case DW_TAG_namelist: return "DW_TAG_namelist";
        case DW_TAG_namelist_item: return "DW_TAG_namelist_item";
        case DW_TAG_packed_type: return "DW_TAG_packed_type";
        case DW_TAG_subprogram: return "DW_TAG_subprogram";
        case DW_TAG_template_type_parameter: return "DW_TAG_template_type_parameter";
        case DW_TAG_template_value_parameter: return "DW_TAG_template_value_parameter";
        case DW_TAG_thrown_type: return "DW_TAG_thrown_type";
        case DW_TAG_try_block: return "DW_TAG_try_block";
        case DW_TAG_variant_part: return "DW_TAG_variant_part";
        case DW_TAG_variable: return "DW_TAG_variable";
        case DW_TAG_volatile_type: return "DW_TAG_volatile_type";
        case DW_TAG_dwarf_procedure: return "DW_TAG_dwarf_procedure";
        case DW_TAG_restrict_type: return "DW_TAG_restrict_type";
        case DW_TAG_interface_type: return "DW_TAG_interface_type";
        case DW_TAG_namespace: return "DW_TAG_namespace";
        case DW_TAG_imported_module: return "DW_TAG_imported_module";
        case DW_TAG_unspecified_type: return "DW_TAG_unspecified_type";
        case DW_TAG_partial_unit: return "DW_TAG_partial_unit";
        case DW_TAG_imported_unit: return "DW_TAG_imported_unit";
        case DW_TAG_condition: return "DW_TAG_condition";
        case DW_TAG_shared_type: return "DW_TAG_shared_type";
        case DW_TAG_type_unit: return "DW_TAG_type_unit";
        case DW_TAG_rvalue_reference_type: return "DW_TAG_rvalue_reference_type";
        case DW_TAG_template_alias: return "DW_TAG_template_alias";
        case DW_TAG_lo_user: return "DW_TAG_lo_user";
        case DW_TAG_hi_user: return "DW_TAG_hi_user";
        default: return "DW_TAG_unknown";
    }
}

static inline const char* dwarf_attr_name(uint64_t attr) {
    switch (attr) {
        case DW_AT_sibling: return "DW_AT_sibling";
        case DW_AT_location: return "DW_AT_location";
        case DW_AT_name: return "DW_AT_name";
        case DW_AT_ordering: return "DW_AT_ordering";
        case DW_AT_byte_size: return "DW_AT_byte_size";
        case DW_AT_bit_offset: return "DW_AT_bit_offset";
        case DW_AT_bit_size: return "DW_AT_bit_size";
        case DW_AT_stmt_list: return "DW_AT_stmt_list";
        case DW_AT_low_pc: return "DW_AT_low_pc";
        case DW_AT_high_pc: return "DW_AT_high_pc";
        case DW_AT_language: return "DW_AT_language";
        case DW_AT_discr: return "DW_AT_discr";
        case DW_AT_discr_value: return "DW_AT_discr_value";
        case DW_AT_visibility: return "DW_AT_visibility";
        case DW_AT_import: return "DW_AT_import";
        case DW_AT_string_length: return "DW_AT_string_length";
        case DW_AT_common_reference: return "DW_AT_common_reference";
        case DW_AT_comp_dir: return "DW_AT_comp_dir";
        case DW_AT_const_value: return "DW_AT_const_value";
        case DW_AT_containing_type: return "DW_AT_containing_type";
        case DW_AT_default_value: return "DW_AT_default_value";
        case DW_AT_inline: return "DW_AT_inline";
        case DW_AT_is_optional: return "DW_AT_is_optional";
        case DW_AT_lower_bound: return "DW_AT_lower_bound";
        case DW_AT_producer: return "DW_AT_producer";
        case DW_AT_prototyped: return "DW_AT_prototyped";
        case DW_AT_return_addr: return "DW_AT_return_addr";
        case DW_AT_start_scope: return "DW_AT_start_scope";
        case DW_AT_bit_stride: return "DW_AT_bit_stride";
        case DW_AT_upper_bound: return "DW_AT_upper_bound";
        case DW_AT_abstract_origin: return "DW_AT_abstract_origin";
        case DW_AT_accessibility: return "DW_AT_accessibility";
        case DW_AT_address_class: return "DW_AT_address_class";
        case DW_AT_artificial: return "DW_AT_artificial";
        case DW_AT_base_types: return "DW_AT_base_types";
        case DW_AT_calling_convention: return "DW_AT_calling_convention";
        case DW_AT_count: return "DW_AT_count";
        case DW_AT_data_member_location: return "DW_AT_data_member_location";
        case DW_AT_decl_column: return "DW_AT_decl_column";
        case DW_AT_decl_file: return "DW_AT_decl_file";
        case DW_AT_decl_line: return "DW_AT_decl_line";
        case DW_AT_declaration: return "DW_AT_declaration";
        case DW_AT_discr_list: return "DW_AT_discr_list";
        case DW_AT_encoding: return "DW_AT_encoding";
        case DW_AT_external: return "DW_AT_external";
        case DW_AT_frame_base: return "DW_AT_frame_base";
        case DW_AT_friend: return "DW_AT_friend";
        case DW_AT_identifier_case: return "DW_AT_identifier_case";
        case DW_AT_macro_info: return "DW_AT_macro_info";
        case DW_AT_namelist_item: return "DW_AT_namelist_item";
        case DW_AT_priority: return "DW_AT_priority";
        case DW_AT_segment: return "DW_AT_segment";
        case DW_AT_specification: return "DW_AT_specification";
        case DW_AT_static_link: return "DW_AT_static_link";
        case DW_AT_type: return "DW_AT_type";
        case DW_AT_use_location: return "DW_AT_use_location";
        case DW_AT_variable_parameter: return "DW_AT_variable_parameter";
        case DW_AT_virtuality: return "DW_AT_virtuality";
        case DW_AT_vtable_elem_location: return "DW_AT_vtable_elem_location";
        case DW_AT_allocated: return "DW_AT_allocated";
        case DW_AT_associated: return "DW_AT_associated";
        case DW_AT_data_location: return "DW_AT_data_location";
        case DW_AT_byte_stride: return "DW_AT_byte_stride";
        case DW_AT_entry_pc: return "DW_AT_entry_pc";
        case DW_AT_use_UTF8: return "DW_AT_use_UTF8";
        case DW_AT_extension: return "DW_AT_extension";
        case DW_AT_ranges: return "DW_AT_ranges";
        case DW_AT_trampoline: return "DW_AT_trampoline";
        case DW_AT_call_column: return "DW_AT_call_column";
        case DW_AT_call_file: return "DW_AT_call_file";
        case DW_AT_call_line: return "DW_AT_call_line";
        case DW_AT_description: return "DW_AT_description";
        case DW_AT_binary_scale: return "DW_AT_binary_scale";
        case DW_AT_decimal_scale: return "DW_AT_decimal_scale";
        case DW_AT_small: return "DW_AT_small";
        case DW_AT_decimal_sign: return "DW_AT_decimal_sign";
        case DW_AT_digit_count: return "DW_AT_digit_count";
        case DW_AT_picture_string: return "DW_AT_picture_string";
        case DW_AT_mutable: return "DW_AT_mutable";
        case DW_AT_threads_scaled: return "DW_AT_threads_scaled";
        case DW_AT_explicit: return "DW_AT_explicit";
        case DW_AT_object_pointer: return "DW_AT_object_pointer";
        case DW_AT_endianity: return "DW_AT_endianity";
        case DW_AT_elemental: return "DW_AT_elemental";
        case DW_AT_pure: return "DW_AT_pure";
        case DW_AT_recursive: return "DW_AT_recursive";
        case DW_AT_signature: return "DW_AT_signature";
        case DW_AT_main_subprogram: return "DW_AT_main_subprogram";
        case DW_AT_data_bit_offset: return "DW_AT_data_bit_offset";
        case DW_AT_const_expr: return "DW_AT_const_expr";
        case DW_AT_enum_class: return "DW_AT_enum_class";
        case DW_AT_linkage_name: return "DW_AT_linkage_name";
        case DW_AT_lo_user: return "DW_AT_lo_user";
        case DW_AT_hi_user: return "DW_AT_hi_user";
        default: return "DW_AT_unknown";
    }
}

static inline const char* dwarf_form_name(uint64_t form) {
    switch (form) {
        case DW_FORM_addr: return "DW_FORM_addr";
        case DW_FORM_block2: return "DW_FORM_block2";
        case DW_FORM_block4: return "DW_FORM_block4";
        case DW_FORM_data2: return "DW_FORM_data2";
        case DW_FORM_data4: return "DW_FORM_data4";
        case DW_FORM_data8: return "DW_FORM_data8";
        case DW_FORM_string: return "DW_FORM_string";
        case DW_FORM_block: return "DW_FORM_block";
        case DW_FORM_block1: return "DW_FORM_block1";
        case DW_FORM_data1: return "DW_FORM_data1";
        case DW_FORM_flag: return "DW_FORM_flag";
        case DW_FORM_sdata: return "DW_FORM_sdata";
        case DW_FORM_strp: return "DW_FORM_strp";
        case DW_FORM_udata: return "DW_FORM_udata";
        case DW_FORM_ref_addr: return "DW_FORM_ref_addr";
        case DW_FORM_ref1: return "DW_FORM_ref1";
        case DW_FORM_ref2: return "DW_FORM_ref2";
        case DW_FORM_ref4: return "DW_FORM_ref4";
        case DW_FORM_ref8: return "DW_FORM_ref8";
        case DW_FORM_ref_udata: return "DW_FORM_ref_udata";
        case DW_FORM_indirect: return "DW_FORM_indirect";
        case DW_FORM_sec_offset: return "DW_FORM_sec_offset";
        case DW_FORM_exprloc: return "DW_FORM_exprloc";
        case DW_FORM_flag_present: return "DW_FORM_flag_present";
        case DW_FORM_ref_sig8: return "DW_FORM_ref_sig8";
        default: return "DW_FORM_unknown";
    }
}

static inline const char* dwarf_op_name(uint64_t op) {
    switch (op) {
        case DW_OP_addr: return "DW_OP_addr";
        case DW_OP_deref: return "DW_OP_deref";
        case DW_OP_const1u: return "DW_OP_const1u";
        case DW_OP_const1s: return "DW_OP_const1s";
        case DW_OP_const2u: return "DW_OP_const2u";
        case DW_OP_const2s: return "DW_OP_const2s";
        case DW_OP_const4u: return "DW_OP_const4u";
        case DW_OP_const4s: return "DW_OP_const4s";
        case DW_OP_const8u: return "DW_OP_const8u";
        case DW_OP_const8s: return "DW_OP_const8s";
        case DW_OP_constu: return "DW_OP_constu";
        case DW_OP_consts: return "DW_OP_consts";
        case DW_OP_dup: return "DW_OP_dup";
        case DW_OP_drop: return "DW_OP_drop";
        case DW_OP_over: return "DW_OP_over";
        case DW_OP_pick: return "DW_OP_pick";
        case DW_OP_swap: return "DW_OP_swap";
        case DW_OP_rot: return "DW_OP_rot";
        case DW_OP_xderef: return "DW_OP_xderef";
        case DW_OP_abs: return "DW_OP_abs";
        case DW_OP_and: return "DW_OP_and";
        case DW_OP_div: return "DW_OP_div";
        case DW_OP_minus: return "DW_OP_minus";
        case DW_OP_mod: return "DW_OP_mod";
        case DW_OP_mul: return "DW_OP_mul";
        case DW_OP_neg: return "DW_OP_neg";
        case DW_OP_not: return "DW_OP_not";
        case DW_OP_or: return "DW_OP_or";
        case DW_OP_plus: return "DW_OP_plus";
        case DW_OP_plus_uconst: return "DW_OP_plus_uconst";
        case DW_OP_shl: return "DW_OP_shl";
        case DW_OP_shr: return "DW_OP_shr";
        case DW_OP_shra: return "DW_OP_shra";
        case DW_OP_xor: return "DW_OP_xor";
        case DW_OP_skip: return "DW_OP_skip";
        case DW_OP_bra: return "DW_OP_bra";
        case DW_OP_eq: return "DW_OP_eq";
        case DW_OP_ge: return "DW_OP_ge";
        case DW_OP_gt: return "DW_OP_gt";
        case DW_OP_le: return "DW_OP_le";
        case DW_OP_lt: return "DW_OP_lt";
        case DW_OP_ne: return "DW_OP_ne";
        case DW_OP_lit0: return "DW_OP_lit0";
        case DW_OP_lit1: return "DW_OP_lit1";
        case DW_OP_lit2: return "DW_OP_lit2";
        case DW_OP_lit3: return "DW_OP_lit3";
        case DW_OP_lit4: return "DW_OP_lit4";
        case DW_OP_lit5: return "DW_OP_lit5";
        case DW_OP_lit6: return "DW_OP_lit6";
        case DW_OP_lit7: return "DW_OP_lit7";
        case DW_OP_lit8: return "DW_OP_lit8";
        case DW_OP_lit9: return "DW_OP_lit9";
        case DW_OP_lit10: return "DW_OP_lit10";
        case DW_OP_lit11: return "DW_OP_lit11";
        case DW_OP_lit12: return "DW_OP_lit12";
        case DW_OP_lit13: return "DW_OP_lit13";
        case DW_OP_lit14: return "DW_OP_lit14";
        case DW_OP_lit15: return "DW_OP_lit15";
        case DW_OP_lit16: return "DW_OP_lit16";
        case DW_OP_lit17: return "DW_OP_lit17";
        case DW_OP_lit18: return "DW_OP_lit18";
        case DW_OP_lit19: return "DW_OP_lit19";
        case DW_OP_lit20: return "DW_OP_lit20";
        case DW_OP_lit21: return "DW_OP_lit21";
        case DW_OP_lit22: return "DW_OP_lit22";
        case DW_OP_lit23: return "DW_OP_lit23";
        case DW_OP_lit24: return "DW_OP_lit24";
        case DW_OP_lit25: return "DW_OP_lit25";
        case DW_OP_lit26: return "DW_OP_lit26";
        case DW_OP_lit27: return "DW_OP_lit27";
        case DW_OP_lit28: return "DW_OP_lit28";
        case DW_OP_lit29: return "DW_OP_lit29";
        case DW_OP_lit30: return "DW_OP_lit30";
        case DW_OP_lit31: return "DW_OP_lit31";
        case DW_OP_reg0: return "DW_OP_reg0";
        case DW_OP_reg1: return "DW_OP_reg1";
        case DW_OP_reg2: return "DW_OP_reg2";
        case DW_OP_reg3: return "DW_OP_reg3";
        case DW_OP_reg4: return "DW_OP_reg4";
        case DW_OP_reg5: return "DW_OP_reg5";
        case DW_OP_reg6: return "DW_OP_reg6";
        case DW_OP_reg7: return "DW_OP_reg7";
        case DW_OP_reg8: return "DW_OP_reg8";
        case DW_OP_reg9: return "DW_OP_reg9";
        case DW_OP_reg10: return "DW_OP_reg10";
        case DW_OP_reg11: return "DW_OP_reg11";
        case DW_OP_reg12: return "DW_OP_reg12";
        case DW_OP_reg13: return "DW_OP_reg13";
        case DW_OP_reg14: return "DW_OP_reg14";
        case DW_OP_reg15: return "DW_OP_reg15";
        case DW_OP_reg16: return "DW_OP_reg16";
        case DW_OP_reg17: return "DW_OP_reg17";
        case DW_OP_reg18: return "DW_OP_reg18";
        case DW_OP_reg19: return "DW_OP_reg19";
        case DW_OP_reg20: return "DW_OP_reg20";
        case DW_OP_reg21: return "DW_OP_reg21";
        case DW_OP_reg22: return "DW_OP_reg22";
        case DW_OP_reg23: return "DW_OP_reg23";
        case DW_OP_reg24: return "DW_OP_reg24";
        case DW_OP_reg25: return "DW_OP_reg25";
        case DW_OP_reg26: return "DW_OP_reg26";
        case DW_OP_reg27: return "DW_OP_reg27";
        case DW_OP_reg28: return "DW_OP_reg28";
        case DW_OP_reg29: return "DW_OP_reg29";
        case DW_OP_reg30: return "DW_OP_reg30";
        case DW_OP_reg31: return "DW_OP_reg31";
        case DW_OP_breg0: return "DW_OP_breg0";
        case DW_OP_breg1: return "DW_OP_breg1";
        case DW_OP_breg2: return "DW_OP_breg2";
        case DW_OP_breg3: return "DW_OP_breg3";
        case DW_OP_breg4: return "DW_OP_breg4";
        case DW_OP_breg5: return "DW_OP_breg5";
        case DW_OP_breg6: return "DW_OP_breg6";
        case DW_OP_breg7: return "DW_OP_breg7";
        case DW_OP_breg8: return "DW_OP_breg8";
        case DW_OP_breg9: return "DW_OP_breg9";
        case DW_OP_breg10: return "DW_OP_breg10";
        case DW_OP_breg11: return "DW_OP_breg11";
        case DW_OP_breg12: return "DW_OP_breg12";
        case DW_OP_breg13: return "DW_OP_breg13";
        case DW_OP_breg14: return "DW_OP_breg14";
        case DW_OP_breg15: return "DW_OP_breg15";
        case DW_OP_breg16: return "DW_OP_breg16";
        case DW_OP_breg17: return "DW_OP_breg17";
        case DW_OP_breg18: return "DW_OP_breg18";
        case DW_OP_breg19: return "DW_OP_breg19";
        case DW_OP_breg20: return "DW_OP_breg20";
        case DW_OP_breg21: return "DW_OP_breg21";
        case DW_OP_breg22: return "DW_OP_breg22";
        case DW_OP_breg23: return "DW_OP_breg23";
        case DW_OP_breg24: return "DW_OP_breg24";
        case DW_OP_breg25: return "DW_OP_breg25";
        case DW_OP_breg26: return "DW_OP_breg26";
        case DW_OP_breg27: return "DW_OP_breg27";
        case DW_OP_breg28: return "DW_OP_breg28";
        case DW_OP_breg29: return "DW_OP_breg29";
        case DW_OP_breg30: return "DW_OP_breg30";
        case DW_OP_breg31: return "DW_OP_breg31";
        case DW_OP_regx: return "DW_OP_regx";
        case DW_OP_fbreg: return "DW_OP_fbreg";
        case DW_OP_bregx: return "DW_OP_bregx";
        case DW_OP_piece: return "DW_OP_piece";
        case DW_OP_deref_size: return "DW_OP_deref_size";
        case DW_OP_xderef_size: return "DW_OP_xderef_size";
        case DW_OP_nop: return "DW_OP_nop";
        case DW_OP_push_object_address: return "DW_OP_push_object_address";
        case DW_OP_call2: return "DW_OP_call2";
        case DW_OP_call4: return "DW_OP_call4";
        case DW_OP_call_ref: return "DW_OP_call_ref";
        case DW_OP_form_tls_address: return "DW_OP_form_tls_address";
        case DW_OP_call_frame_cfa: return "DW_OP_call_frame_cfa";
        case DW_OP_bit_piece: return "DW_OP_bit_piece";
        case DW_OP_implicit_value: return "DW_OP_implicit_value";
        case DW_OP_stack_value: return "DW_OP_stack_value";
        case DW_OP_lo_user: return "DW_OP_lo_user";
        case DW_OP_hi_user: return "DW_OP_hi_user";
        default: return "DW_OP_unknown";
    }
}

#endif // __MY_DWARF_MACROS_H__
