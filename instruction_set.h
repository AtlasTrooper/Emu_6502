//
// Created by Atlas on 11/20/25.
//

#ifndef EMU_6502_INSTRUCTION_SET_H
#define EMU_6502_INSTRUCTION_SET_H

#define INSTRUCTION_SET_SIZE 256
#define I(op, cyc, fn, mode) { op, cyc, fn, mode }
#define ERR { "", 0, NOP, implied }   // illegal / unused opcode


enum ADDR_MODE {
    implied,
    immediate,
    accumulator,
    absolute,
    zeroPage,
    absoluteX,
    absoluteY,
    indirect,
    xIndexedIndirect,
    indirectYIndexed,
    relative,
    zerPageX,
    zerPageY,
};

typedef struct {
    // Opcode hex num, cycleCount, ptr to function, addr mode
    char opcode[10];
    int cycles_count;
    void (*instruction_ptr)(void);

    enum ADDR_MODE addr_mode;
} cpu_instruction;

void LDA();
void LDX();
void LDY();
void STA();
void STX();
void STY();
void TAX();
void TAY();
void TSX();
void TXA();
void TXS();
void TYA();

void OR();
void AND();
void XOR();
void DEC();
void DEX();
void DEY();

void INC();
void INX();
void INY();
void ADC();
void SBC();
void ASL();
void LSR();
void ROL();
void ROR();

void Push(uint8_t);
uint8_t Pop();

void Push16(uint16_t);
uint16_t Pop16();

void PHA();
void PHP();
void PLA();
void PLP();

void BIT();
void NOP();

void BRANCH_IF(int);
void BCC();
void BCS();
void BEQ();
void BNE();
void BMI();
void BPL();
void BVS();
void BVC();

void JMP();
void JSR();
void RTS();

void BRK();
void RTI();

void CLC();
void CLD();
void CLI();
void CLV();
void SEC();
void SED();
void SEI();

void CMP();
void CPX();
void CPY();





/*
 * List of instructions detailing opcode, cycle count and instruction pointer
 * should also match instruction to addressing mode
 */
#pragma region instruction_set_function_table
cpu_instruction instruction_set[INSTRUCTION_SET_SIZE] = {

/* 0x00 */ I("00",7,BRK,implied),          I("01",6,OR,xIndexedIndirect), ERR, ERR,
/* 0x04 */ ERR,                             I("05",3,OR,zeroPage),         I("06",5,ASL,zeroPage), ERR,
/* 0x08 */ I("08",3,PHP,implied),          I("09",2,OR,immediate),        I("0A",2,ASL,accumulator), ERR,
/* 0x0C */ ERR,                             I("0D",4,OR,absolute),         I("0E",6,ASL,absolute), ERR,

/* 0x10 */ I("10",2,BPL,relative),         I("11",5,OR,indirectYIndexed), ERR, ERR,
/* 0x14 */ ERR,                             I("15",4,OR,zerPageX),         I("16",6,ASL,zerPageX), ERR,
/* 0x18 */ I("18",2,CLC,implied),          I("19",4,OR,absoluteY),        ERR, ERR,
/* 0x1C */ ERR,                             I("1D",4,OR,absoluteX),        I("1E",7,ASL,absoluteX), ERR,

/* 0x20 */ I("20",6,JSR,absolute),         I("21",6,AND,xIndexedIndirect), ERR, ERR,
/* 0x24 */ I("24",3,BIT,zeroPage),         I("25",3,AND,zeroPage),        I("26",5,ROL,zeroPage), ERR,
/* 0x28 */ I("28",4,PLP,implied),          I("29",2,AND,immediate),       I("2A",2,ROL,accumulator), ERR,
/* 0x2C */ I("2C",4,BIT,absolute),         I("2D",4,AND,absolute),        I("2E",6,ROL,absolute), ERR,

/* 0x30 */ I("30",2,BMI,relative),         I("31",5,AND,indirectYIndexed), ERR, ERR,
/* 0x34 */ ERR,                             I("35",4,AND,zerPageX),         I("36",6,ROL,zerPageX), ERR,
/* 0x38 */ I("38",2,SEC,implied),          I("39",4,AND,absoluteY),       ERR, ERR,
/* 0x3C */ ERR,                             I("3D",4,AND,absoluteX),       I("3E",7,ROL,absoluteX), ERR,

/* 0x40 */ I("40",6,RTI,implied),          I("41",6,XOR,xIndexedIndirect), ERR, ERR,
/* 0x44 */ ERR,                             I("45",3,XOR,zeroPage),        I("46",5,LSR,zeroPage), ERR,
/* 0x48 */ I("48",3,PHA,implied),          I("49",2,XOR,immediate),       I("4A",2,LSR,accumulator), ERR,
/* 0x4C */ I("4C",3,JMP,absolute),         I("4D",4,XOR,absolute),        I("4E",6,LSR,absolute), ERR,

/* 0x50 */ I("50",2,BVC,relative),         I("51",5,XOR,indirectYIndexed), ERR, ERR,
/* 0x54 */ ERR,                             I("55",4,XOR,zerPageX),         I("56",6,LSR,zerPageX), ERR,
/* 0x58 */ I("58",2,CLI,implied),          I("59",4,XOR,absoluteY),       ERR, ERR,
/* 0x5C */ ERR,                             I("5D",4,XOR,absoluteX),       I("5E",7,LSR,absoluteX), ERR,

/* 0x60 */ I("60",6,RTS,implied),          I("61",6,ADC,xIndexedIndirect), ERR, ERR,
/* 0x64 */ ERR,                             I("65",3,ADC,zeroPage),        I("66",5,ROR,zeroPage), ERR,
/* 0x68 */ I("68",4,PLA,implied),          I("69",2,ADC,immediate),       I("6A",2,ROR,accumulator), ERR,
/* 0x6C */ I("6C",5,JMP,indirect),         I("6D",4,ADC,absolute),        I("6E",6,ROR,absolute), ERR,

/* 0x70 */ I("70",2,BVS,relative),         I("71",5,ADC,indirectYIndexed), ERR, ERR,
/* 0x74 */ ERR,                             I("75",4,ADC,zerPageX),         I("76",6,ROR,zerPageX), ERR,
/* 0x78 */ I("78",2,SEI,implied),          I("79",4,ADC,absoluteY),       ERR, ERR,
/* 0x7C */ ERR,                             I("7D",4,ADC,absoluteX),       I("7E",7,ROR,absoluteX), ERR,

/* 0x80 */ ERR,                             I("81",6,STA,xIndexedIndirect), ERR, ERR,
/* 0x84 */ I("84",3,STY,zeroPage),         I("85",3,STA,zeroPage),        I("86",3,STX,zeroPage), ERR,
/* 0x88 */ I("88",2,DEY,implied),          ERR,                             I("8A",2,TXA,implied), ERR,
/* 0x8C */ I("8C",4,STY,absolute),         I("8D",4,STA,absolute),        I("8E",4,STX,absolute), ERR,

/* 0x90 */ I("90",2,BCC,relative),         I("91",6,STA,indirectYIndexed), ERR, ERR,
/* 0x94 */ I("94",4,STY,zerPageX),         I("95",4,STA,zerPageX),        I("96",4,STX,zerPageY), ERR,
/* 0x98 */ I("98",2,TYA,implied),          I("99",5,STA,absoluteY),       I("9A",2,TXS,implied), ERR,
/* 0x9C */ ERR,                             I("9D",5,STA,absoluteX),       ERR, ERR,

/* 0xA0 */ I("A0",2,LDY,immediate),        I("A1",6,LDA,xIndexedIndirect), I("A2",2,LDX,immediate), ERR,
/* 0xA4 */ I("A4",3,LDY,zeroPage),         I("A5",3,LDA,zeroPage),        I("A6",3,LDX,zeroPage), ERR,
/* 0xA8 */ I("A8",2,TAY,implied),          I("A9",2,LDA,immediate),       I("AA",2,TAX,implied), ERR,
/* 0xAC */ I("AC",4,LDY,absolute),         I("AD",4,LDA,absolute),        I("AE",4,LDX,absolute), ERR,

/* 0xB0 */ I("B0",2,BCS,relative),         I("B1",5,LDA,indirectYIndexed), ERR, ERR,
/* 0xB4 */ I("B4",4,LDY,zerPageX),         I("B5",4,LDA,zerPageX),        I("B6",4,LDX,zerPageY), ERR,
/* 0xB8 */ I("B8",2,CLV,implied),          I("B9",4,LDA,absoluteY),       I("BA",2,TSX,implied), ERR,
/* 0xBC */ I("BC",4,LDY,absoluteX),        I("BD",4,LDA,absoluteX),       I("BE",4,LDX,absoluteY), ERR,

/* 0xC0 */ I("C0",2,CPY,immediate),        I("C1",6,CMP,xIndexedIndirect), ERR, ERR,
/* 0xC4 */ I("C4",3,CPY,zeroPage),         I("C5",3,CMP,zeroPage),        I("C6",5,DEC,zeroPage), ERR,
/* 0xC8 */ I("C8",2,INY,implied),          I("C9",2,CMP,immediate),       I("CA",2,DEX,implied), ERR,
/* 0xCC */ I("CC",4,CPY,absolute),         I("CD",4,CMP,absolute),        I("CE",6,DEC,absolute), ERR,

/* 0xD0 */ I("D0",2,BNE,relative),         I("D1",5,CMP,indirectYIndexed), ERR, ERR,
/* 0xD4 */ ERR,                             I("D5",4,CMP,zerPageX),         I("D6",6,DEC,zerPageX), ERR,
/* 0xD8 */ I("D8",2,CLD,implied),          I("D9",4,CMP,absoluteY),       ERR, ERR,
/* 0xDC */ ERR,                             I("DD",4,CMP,absoluteX),       I("DE",7,DEC,absoluteX), ERR,

/* 0xE0 */ I("E0",2,CPX,immediate),        I("E1",6,SBC,xIndexedIndirect), ERR, ERR,
/* 0xE4 */ I("E4",3,CPX,zeroPage),         I("E5",3,SBC,zeroPage),        I("E6",5,INC,zeroPage), ERR,
/* 0xE8 */ I("E8",2,INX,implied),          I("E9",2,SBC,immediate),       I("EA",2,NOP,implied), ERR,
/* 0xEC */ I("EC",4,CPX,absolute),         I("ED",4,SBC,absolute),        I("EE",6,INC,absolute), ERR,

/* 0xF0 */ I("F0",2,BEQ,relative),         I("F1",5,SBC,indirectYIndexed), ERR, ERR,
/* 0xF4 */ ERR,                             I("F5",4,SBC,zerPageX),         I("F6",6,INC,zerPageX), ERR,
/* 0xF8 */ I("F8",2,SED,implied),          I("F9",4,SBC,absoluteY),       ERR, ERR,
/* 0xFC */ ERR,                             I("FD",4,SBC,absoluteX),       I("FE",7,INC,absoluteX), ERR

};
#pragma endregion
#endif //EMU_6502_INSTRUCTION_SET_H
