//
// Created by Atlas on 11/1/25.
//

#ifndef EMU_6502_QOL_H
#define EMU_6502_QOL_H

#pragma  once
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "instruction_set.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

//Flag positions
u8 N_FLAG = 0x1;
u8 V_FLAG = 0x2;
u8 B_FLAG = 0x8;
u8 D_FLAG = 0x10;
u8 I_FLAG = 0x20;
u8 Z_FLAG = 0x40;
u8 C_FLAG = 0x80;


#define LINE printf("---------\n");

#endif //EMU_6502_QOL_H