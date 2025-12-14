//
// Created by Atlas on 11/1/25.
//

#ifndef EMU_6502_QOL_H
#define EMU_6502_QOL_H

#pragma  once
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
//#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "instruction_set.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

//Flag positions
const u8 C_FLAG = 0x01;
const u8 Z_FLAG = 0x02;
const u8 I_FLAG = 0x08;
const u8 D_FLAG = 0x10;
const u8 B_FLAG = 0x20;
const u8 V_FLAG = 0x40;
const u8 N_FLAG = 0x80;

enum FLAGS {
    N,
    V,
    B,
    D,
    I,
    Z,
    C
};


#define LINE printf("---------\n");

#endif //EMU_6502_QOL_H
