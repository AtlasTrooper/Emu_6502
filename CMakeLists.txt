cmake_minimum_required(VERSION 4.0)
project(Emu_6502 C)

set(CMAKE_C_STANDARD 90)

add_executable(Emu_6502 main.c
        qol.h)

