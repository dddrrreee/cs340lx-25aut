# Parsing the DWARF Debugging Information Format (*Lab by Stuart Sul*)

![Cute banner image](./images/DWARF-factory.png)

<center>
    <b><i>Work in progress</i></b><br/>
    <b><i>Work in progress</i></b><br/>
    <b><i>Work in progress</i></b><br/>
    <b><i>Work in progress</i></b><br/>
    <b><i>Work in progress</i></b><br/>
    <b><i>Work in progress</i></b><br/>
</center>

Remember the [ELF lab](https://github.com/dddrrreee/cs240lx-25spr/tree/main/labs/15-elf-dynamic-linker) 🥹? We parsed through many ELF sections, except for a few... the ones that started with `.debug_*`. These are part of the [DWARF Debugging Information Format Standard](https://dwarfstd.org/), a separate format designed alongside ELF.

Well, today we *completely* demystify the ELF by parsing those sections too! Yes, this is the **ELF lab, part 2**. We’ll build a DWARF parser that can find various metadata of a binary at runtime, letting us build our own full custom debugger (which was my 240LX final project)! And all this still on bare-metal hardware, without an OS.

This will uncover what modern debuggers like `gdb` actually do: how they set breakpoints, or print the current file name and line number. Again, it’s going to be a lot of fun! Yay! 🥳

**Check-off: Finish parts 0, 1, 2, and 3**

## Recap 1: How we run programs on our Raspberry Pi

Throughout CS140E, CS240LX, and CS340LX we've been statically linking everything, and passed the raw instructions to our R/pi through our bootloader. If you've been paying attention to the Makefiles we use in the labs, you could've seen that the process is roughly:

1. Compile all the source codes under `./libpi` directory and whatever lab directory you are in with `arm-none-eabi-gcc` ➡️ this produces individual object files (`*.o`), which are ELF files
2. Link together all the object files created under `./libpi` into a static library with `arm-none-eabi-ar` (archiver) ➡️ this produces `libpi.a`, a collection of ELF files
3. Link together `libpi.a` and all the object files created under your lab directory into an executable with `arm-none-eabi-ld` (linker) ➡️ this produces an executable (ex. `my-test.elf`), also an ELF file
4. Strip away all ELF/linker metadata and only keep the raw `.text`, `.rodata`, and `.data` sections from the above ELF executable with `arm-none-eabi-objcopy -O binary` ➡️ no longer an ELF file 😥 we send THIS to the pi-side bootloader, which loads the binary into memory and jumps to 0x8000

In the [CS240LX ELF and Dynamic Linker Lab](https://github.com/dddrrreee/cs240lx-25spr/tree/main/labs/15-elf-dynamic-linker), we built our own ELF parser and dynamic linker to overcome the downsides of this approach: the final executable being redundant with our pi-side bootloader, inability to directly run ELF files (i.e., always have to port things into our build system), inability to use shared libraries (because we can't link them!), and having to always statically link everything in advance. As a result, we upgraded our pi-side bootloader to a fully-fledged ELF32 parser and dynamic loader/linker.

Well today, we don't touch our bootloaders. Rather than an ELF32 parser and dynamic loader/linker, we build an **ELF32 and DWARF parser**. To do this, we demystify the part of ELF file that we ignored in our previous ELF lab: the debugging information sections. With this, we are able to obtain all sorts of information that we need to debug our programs during runtime: file and line numbers, location of variables, function arguments, and much more! This will power you to do things like building your own addr2line program, your own GDB, or anything crazier you can imagine.

## Recap 2: Executable and Linkable Format (ELF)

The [Wikipedia page](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format) is helpful, as well as [this GitHub page](https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779). But if you don't have time, looking at the provided [reference sheet](./docs/ELF.pdf) under this directory is sufficient.

Basically, ELF is the standard binary format on many Unix-based and Unix-like operating systems. As its name suggests, it is used to represent executable files (ex. `my_program`), as well as object code (ex. `my_program.o`) and shared libraries (ex. `libpi.a` or `libc.so`). Why use a common format for binaries instead of directly writing machine instructions to binary files? Portability. The operating system can use ELF headers to determine how to parse different types of binaries, or to reject a binary as uninterpretable based on its metadata. ELF metadata also provides other useful information, such as the binary's endianness and its entry point (the address where the OS should begin execution of the program).

The basic structure of an ELF file is fairly simple; it consists of **4 main parts**:

```plaintext
    ----------------------------------
    |            ELF Header          |
    ----------------------------------
    |      Program Header Table      |
    ----------------------------------
    |           Sections             |
    |  (.text, .data, .symtab, ...)  |
    ----------------------------------
    |      Section Header Table      |
    ----------------------------------
```

Here's a more fancy, detailed diagram:

![ELF Diagram](./images/ELF-format.png)

The **ELF Header** is the first component of an ELF file and contains general metadata such as the magic number, architecture (e.g., x86 or ARM), address size (32-bit or 64-bit), endianness, entry point address, and offsets to the program and section header tables.

The **Program Header Table** describes how to load the ELF file into memory. Each entry in the program header table corresponds to a *segment* and tells the loader which parts of the file should be mapped into memory, where in the virtual memory space they should be loaded, what permissions they require (read, write, execute), and their size in the file and in memory. This table is essential during runtime but not during compile-time. 

If we were being really precise, we would use the R/pi’s virtual memory system to map segments and enforce access controls according to the program header table. This is useful because it allows us to avoid loading unnecessary parts of the ELF binary (e.g., `.symtab`), prevent execution of non-executable segments, and etc. This would make a great extension--we already have all the pieces needed. But for today, *we ignore the program header table*.

**Sections** are the actual contents of the binary: code, data, symbols, relocation information, and much more. Common sections include `.text`, `.data`, `.bss`, `symtab`, etc.

The **Section Header Table** describes each section in the ELF file: section name (e.g., `.text`), type, memory address, size, and offset in the file. This is what you use to actually locate the individual sections. It is mainly used by linkers and debuggers.

Our workflow with ELF for today is fairly simple, similar to the previous ELF lab:

1. Read the ELF header ➡️ We verify that the file we are reading is in fact an ELF file, locate the section header table, and find the program entry point.
2. Read the section header table ➡️ We locate the sections we are interested in (specifically, `.debug_info`, `debug_abbrev`, `.debug_aranges`, `.debug_line`, `.debug_str`, `.debug_frame`, `.debug_loc`, and `.debug_ranges`).
3. Read each section ➡️ Extract whatever information we need.
4. Do anything you want to do with the extracted information.

## Background: DWARF Debugging Information Format

DWARF is a widely used, standardized debugging data format originally developed alongside the Executable and Linkable Format (ELF), though it operates independently of any object file format. Its name, chosen as a medieval fantasy complement to “ELF,” has no official meaning. Designed for source-level debugging, DWARF supports a range of procedural languages such as C, C++, and Fortran, and its extensible structure allows compatibility with additional languages. It is architecture-independent, making it suitable for any processor or operating system, and is extensively used across Unix, Linux, and other systems, including stand-alone environments.

## Part 0: Compile the ELF Executable Files and Move Them to Your SD Card (3 min)

**Check-off:**
- Run `make` inside `0-my-libpi` and `1-my-tests`, in that order.
- Copy the resulting ELF files from `1-my-tests` to your SD card
- Read the `*.elf.readelf` files and locate the debugging information sections.

Just as we did in the previous ELF lab, It is 😡😡😡 to repeatedly move files between your host machine and R/Pi. So we'll move everything we need now, and never remove the SD card again.

First, we want our ELF executable binaries to be consistent across environments for a smooth lab. So instead of relying on the individual `libpi` directory on everyone's computer, this lab has a special `0-my-libpi` directory which later builds will rely on. Unlike the previous ELF lab, we do not create `libpi.so` and dynamically link it to our program. We just generate a `libpi.a` to be statically linked later. This step might be unnecessary, but better than having to deal with potential bugs on everyone's different development environment. So go ahead and run `make` inside `0-my-libpi`. This should produce a `libpi.a` archive. No further actions are needed on this directory.

Next, `cd` into `1-my-tests`. This directory contains 3 programs which we will compile as full ELF executables, move into our SD card, and later parse to obtain debugging information about them. We never run these programs in today's lab (although it would be a great extension to do that too). Instead, we run our DWARF parser, which will read these binaries from the FAT32 filesystem, parse the debugging information sections, and obtain metadata about the binary, such as which address corresponds to which line number of the source file. So run `make` inside `1-my-tests` and move all `*.elf` files (`test-0.elf`, `test-1.elf`, and `test-2.elf`) into your SD card. Make sure to run `sync` before you eject your SD card so that all data is properly written to disk.

Finally, it is beneficial to actually look and see what sections we will be actually parsing today. So open up one of the `*.elf.readelf` files in `1-my-tests` after compiling the programs. These are generated output from running `readelf` program on the ELF executables. For instance, opening up `test-0.elf.readelf`, you should be seeing:

```plaintext
Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        00000500 000500 000a40 00  AX  0   0  4
  [ 2] .rodata           PROGBITS        00000f40 000f40 0000f5 00   A  0   0  4
  [ 3] .data             PROGBITS        00001038 001038 000004 00  WA  0   0  4
  [ 4] .got              PROGBITS        0000103c 00103c 000008 00  WA  0   0  4
  [ 5] .got.plt          PROGBITS        00001044 001044 00000c 04  WA  0   0  4
  [ 6] .bss              NOBITS          00001050 001050 000008 00  WA  0   0  4
  [ 7] .debug_info       PROGBITS        00000000 001050 006d92 00      0   0  1
  [ 8] .debug_abbrev     PROGBITS        00000000 007de2 0017e2 00      0   0  1
  [ 9] .debug_aranges    PROGBITS        00000000 0095c4 000160 00      0   0  1
  [10] .debug_line       PROGBITS        00000000 009724 00129c 00      0   0  1
  [11] .debug_str        PROGBITS        00000000 00a9c0 000b86 01  MS  0   0  1
  [12] .comment          PROGBITS        00000000 00b546 000079 01  MS  0   0  1
  [13] .ARM.attributes   ARM_ATTRIBUTES  00000000 00b5bf 00002b 00      0   0  1
  [14] .debug_frame      PROGBITS        00000000 00b5ec 000400 00      0   0  4
  [15] .debug_loc        PROGBITS        00000000 00b9ec 0006af 00      0   0  1
  [16] .debug_ranges     PROGBITS        00000000 00c09b 000060 00      0   0  1
  [17] .symtab           SYMTAB          00000000 00c0fc 0008a0 10     18  88  4
  [18] .strtab           STRTAB          00000000 00c99c 000306 00      0   0  1
  [19] .shstrtab         STRTAB          00000000 00cca2 0000c1 00      0   0  1
```

If you remember the previous ELF lab, we had to parse multiple of these sections in order to parse the ELF file and dynamically link shared libraries. That included: `.text`, `.rodata`, `.data`, `.bss`, `.strtab` for normal program execution, and `.got`, `.got.plt`, `.symtab`, and numerous dynamic sections (not generated here because the binaries are statically linked!) for dynamic linking. 

However, we did not have all these `.debug_*` sections during that lab. Why did they appear? It's because we passed `--strip-debug` flag to the linker and stripped away all the debugging sections. But in today's lab, these sections are our main characters. They contain all sorts of debugging metadata about the executable, and we will parse them to extract all sorts of useful information!

## Part 1: Load the DWARF Sections into Memory

**Check-off:**

* asdf
* asdf

## Part 2: 

**Check-off:**

* asdf
* asdf

## Part 3: 

**Check-off:**

* asdf
* asdf

## Extensions

asdf
