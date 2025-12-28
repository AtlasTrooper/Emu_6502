# MOS 6502 Emulator (C)

Hello! 👋  
This repository contains a simple (and admittedly somewhat crude) implementation of a **MOS 6502 CPU emulator written in C**.

At its current stage, the emulator is capable of executing **all “legal” 6502 instructions** included in the original instruction set. The focus of this project is correctness and clarity rather than cycle-perfect emulation or system-specific behavior.

---

## Features

- Full support for all *legal* MOS 6502 instructions  
- Minimal dependencies  
- Portable, standard C implementation  
- Designed as a generic CPU emulator core  

---

## Setup & Usage

### Requirements

- Any standard C compiler (e.g. `gcc`, `clang`)
- Standard C libraries such as (`stdlib`, `stdio`, etc

### Build

```bash

gcc main.c -o emulator

./emulator <path-to-rom>.6502

```
## Important Notes

This emulator is designed to be a **generic MOS 6502 CPU emulator**, not a system-specific implementation.

As a result:

- It **will not work out of the box with every cartridge or ROM format**
- System-specific behavior (memory mapping, I/O, peripherals) is **not implemented by default**
- The `bus_read` and `bus_write` functions are currently implemented **only for the custom test ROMs** written to validate this emulator

If you intend to emulate a specific system (such as the NES, Commodore 64, or Apple II), you will need to modify these bus functions to reflect that system’s memory map and hardware behavior.

