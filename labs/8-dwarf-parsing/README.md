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

## Background 1: Executable and Linkable Format (ELF)

First, the CS 240LX ELF lab is a prerequisite: [CS240LX: Executable and Linkable Format (ELF) and Dynamic Linker](https://github.com/dddrrreee/cs240lx-25spr/tree/main/labs/15-elf-dynamic-linker)

## Background 2: DWARF Debugging Information Format

DWARF is a widely used, standardized debugging data format originally developed alongside the Executable and Linkable Format (ELF), though it operates independently of any object file format. Its name, chosen as a medieval fantasy complement to “ELF,” has no official meaning. Designed for source-level debugging, DWARF supports a range of procedural languages such as C, C++, and Fortran, and its extensible structure allows compatibility with additional languages. It is architecture-independent, making it suitable for any processor or operating system, and is extensively used across Unix, Linux, and other systems, including stand-alone environments.

## Part 0: 

**Check-off:**

* asdf
* asdf

## Part 1: 

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
